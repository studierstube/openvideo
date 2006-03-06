/* ========================================================================
 * Copyright (C) 2005  Graz University of Technology
 *
 * This framework is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This framework is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this framework; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * For further information please contact Denis Kalkofen under
 * <kalkofen@icg.tu-graz.ac.at> or write to Denis Kalkofen,
 * Graz University of Technology, Inffeldgasse 16a, A8010 Graz,
 * Austria.
 * ========================================================================
 * PROJECT: OpenVideo
 * ======================================================================== */
/** The source file for the Manager class.
 *
 * @author Denis Kalkofen
 * 
 * $Id$
 * @file                                                                   
 /* ======================================================================= */
#include <openvideo/Manager.h>

#include <ace/Condition_Thread_Mutex.h>
#include <ace/Thread_Mutex.h>
// The configOV.h file is only used on windows. On linux command line
// arguments are used instead.
#ifdef HAVE_CONFIGOV_H
#include <openvideo/configOV.h>
#endif

#include <tinyxml/tinyxml.h>

#include <openvideo/NodeFactory.h>
#include <openvideo/Node.h>
	
using namespace openvideo;

/////known nodes
#ifdef ENABLE_GLUTSINK
#include <openvideo/nodes/GLUTSinkFactory.h>
#endif

#ifdef ENABLE_TESTSRC
#include <openvideo/nodes/TestSrcFactory.h>
#endif

#ifdef ENABLE_GL_TEXTURE_2D_SINK
#include <openvideo/nodes/GL_TEXTURE_2D_SinkFactory.h>
#endif

#ifdef ENABLE_VIDEOWRAPPERSRC
#include <openvideo/nodes/VideoWrapperSrcFactory.h>
#endif

#ifdef ENABLE_VIDEOSINK
#include <openvideo/nodes/VideoSinkFactory.h>
#endif

#ifdef ENABLE_SPECTECSRC
#include <openvideo/nodes/SpectecSrcFactory.h>
#endif

#ifdef ENABLE_OPENCV
#include <openvideo/nodes/OpenCVSrcFactory.h>
#endif

#include <iostream>

void (*Manager::traversalFunc)	  (void*)=NULL;
void (*Manager::initTraversalFunc)(void*)=NULL;
void* Manager::traversalData=NULL;
void* Manager::initTraversalData=NULL;
bool  Manager::travBlock=false;
bool  Manager::hasGLContext=false;
Manager* Manager::instance=NULL;

Manager::Manager()
{
  logger=  new  Logger();
  scheduler=new Scheduler();
  initNodeFactories();
	
  setInitTravFunction(&Manager::initTopologicalSortedTraversal,&(nodes));
  setTravFunction(&Manager::topologicalSortedTraversal,&(nodes));
  isRunning=true;
  updating=false;
  hasParsed=false;
  updateLock=new ACE_Thread_Mutex();
  updateLockCondition=new ACE_Condition_Thread_Mutex(*updateLock);
  idleSetGLContext=false;
  idleDeleteGLContext=false;
  dc=NULL;
  glContext=NULL;
#ifdef LINUX
  dsp=NULL;
#endif
}

// Destructor method.
Manager::~Manager()
{
  //clean up graph
  for(int i=(int)(nodes.size())-1;i>=0;i--)
    {
      //visit node i
      delete nodes.at(i);
    }
  nodes.clear();
  delete logger;
  delete scheduler;
}

void 
Manager::update(void*)
{
   Manager* self=Manager::getInstance();
   self->updateLock->acquire();
   if(self->isRunning)
   {
       self->updating=true;
       (*Manager::traversalFunc)(Manager::traversalData);
       self->updating=false;
   }
   else
   {
       self->doIdleTasks();
   }
   self->updateLockCondition->broadcast();
   self->updateLock->release();
}

void 
Manager::doIdleTasks()
{
    /////////////////////////////////// set glcontext ///////////////////////////////
    if(idleSetGLContext)
    {
        idleSetGLContext=false;
		if(dc && glContext)
		{
            
#ifdef WIN32
          hasGLContext=wglMakeCurrent(dc,glContext);
#endif
#ifdef LINUX
          hasGLContext=glXMakeCurrent(dsp,drawable,ovGLContext);
#endif
            if(!hasGLContext)
                logger->logEx("OpenVideo: couldn't set glContext\n");
            else
                logger->logEx("OpenVideo: successfully set glContext\n") ;
		}
    }
    /////////////////////////////////// delete glcontext ///////////////////////////////
    else if(idleDeleteGLContext)
    {
      
        idleDeleteGLContext=false;   
        hasGLContext=(!wglDeleteContext(glContext));
        if(hasGLContext)
            logger->logEx("OpenVideo: couldn't delete glContext\n");
        else
            logger->logEx("OpenVideo: successfully deleted glContext\n") ;
            //try to set a new glcontext
           
        
    }


    resume();
}


void 
Manager::pause()
{
    updateLock->acquire();
    if(isRunning)
    {
        isRunning =false;
        updateLockCondition->wait();      
    }
    
    updateLock->release();
}

#ifdef WIN32
    void 
    Manager::setGLContext(HGLRC _glContext,HDC _dc)
    {
        printf("    Manager::setGLContext(HGLRC _glContext,HDC _dc) \n");
        glContext=_glContext;
        dc=_dc;
        idleSetGLContext=true;
        pause();
    }
#endif

#ifdef LINUX
    void 
    Manager::setGLContext(GLXDrawable _drawable, GLXContext _ovGLContext, Display* _dsp)
    {
        dc=_drawable;
        glContext=_ovGLContext
        dsp=_dsp;
        pause();
    }

#endif

void
Manager::deleteGLContext()
{
    idleDeleteGLContext=true;
    pause();
}
void 
Manager::resume()
{
    updateLock->acquire();
    isRunning=true;
    updateLock->release();
}

bool
Manager::isStarted()
{
  return isRunning;
}

Manager* 
Manager::getInstance()
{
  if(!Manager::instance)
    Manager::instance=new Manager();

  return Manager::instance;
		

}

void
Manager::buildSubGraph(TiXmlElement * parentElement, Node* parentNode)
{
  TiXmlElement * element = parentElement->FirstChildElement();
  while(element)
    {
      Node* curNode=addNode(element);

      parentNode->addOutput(curNode);
      curNode->addInput(parentNode);

      buildSubGraph(element,curNode);
      element = element->NextSiblingElement();
    }
}

Node* 
Manager::addNode(TiXmlElement *element)
{
  //search for attribute "use"

  TiXmlAttribute* attribute = element->FirstAttribute();
  bool isUse=false;
  std::string useName="";
  while(attribute)
    {
      if(!strcmp(attribute->Name(),"USE"))
        {
          //node is just referenced 
          isUse=true;
          useName=attribute->Value();
          break;
        }
      attribute = attribute->Next();
    }
	
  Node* curNode=NULL;
  if(isUse){
    //get node from defNodes
    std::vector<Node*>::iterator it;
    for(it=defNodes.begin();it!=defNodes.end();it++){
      Node* defNode=(*it);
      if(!strcmp(defNode->getDefName(),useName.c_str()))
        {
          curNode=defNode;
          break;
        }
    }
  }
  else{
    //find factory
    NodeFactory *nodeFactory=NULL;
    for(int i=0;i<(int)(factories.size());i++)
      {
        if(!strcmp(factories[i]->getNodeTypeId(),element->Value())){
          nodeFactory=factories[i];
          break;
        }
      }
    if(!nodeFactory){
      logger->logEx("OpenVideo: can't find a factory for %s\n",element->Value());
      exit(-1);
    }
    //create Node 
		
    curNode=nodeFactory->createNode();
    nodes.push_back(curNode);
  }//if(isUse)..else
		
  //get attributesj
  attribute = element->FirstAttribute();
  while(attribute)
    {
      if(!strcmp(attribute->Name(),"DEF")){
        defNodes.push_back(curNode);
      }
		
      curNode->setParameter(attribute->Name(), attribute->Value());
      attribute = attribute->Next();
    }

  return curNode;
}

void
Manager::parseConfiguration(TiXmlElement* element)
{
  scheduler->parseConfiguration(element);
}

// parse the xml file and build the graph.
bool
Manager::parseConfiguration(const std::string& filename)
{   
	if (hasParsed)
		return false;
  TiXmlDocument* document = new TiXmlDocument();

  if(!document->LoadFile(filename.c_str()))
    {
      logger->logEx("OpenVideo:An error occured during parsing\n   Message: %s\n", document->ErrorDesc());
      return false;
    }
  //
  TiXmlElement* root = document->RootElement();
  parseConfiguration(root);
  TiXmlElement* element = root->FirstChildElement();
  while(element)
    {
      Node* curNode=addNode(element);
      buildSubGraph(element,curNode);
      element = element->NextSiblingElement();
    }
  //
  /////	parsing is done
  document->Clear();
  delete document;

  hasParsed=true;
  return true;
}

void 
Manager::setTravFunction(void (*travFunction)(void*),void *data)
{
  Manager::traversalFunc=travFunction;
  Manager::traversalData=data;
}

void 
Manager::setInitTravFunction(void (*initTravFunction)(void*),void* data)
{	
  Manager::initTraversalFunc=initTravFunction;
  Manager::initTraversalData=data;
}

void 
Manager::run()
{
  scheduler->init();
  scheduler->run();
}

void 
Manager::stop()
{
  scheduler->stop();
}


void 
Manager::initTraversal()
{
  //validate pixel format
  for(int i=0;i<(int)nodes.size();i++)
    {
      nodes[i]->initPixelFormats();
      if(!nodes[i]->validateCurrentPixelFormat())
        {
          logger->logEx("OV: %s uses an unknown pixel format\n",nodes[i]->getName());
          exit(-1);
        }
    }
  (*Manager::initTraversalFunc)(initTraversalData);
}

void 
Manager::initTopologicalSortedTraversal(void* nodeList)
{
  std::vector<Node *>* nodes=(std::vector<Node *>*)nodeList;
  std::vector<Node *> sortedNodes;
  //sort nodes
  int i;
  while((int)nodes->size()>0)
    {
      //find node with in degree==0
      for (i=0;i<(int)nodes->size();i++){
        if(nodes->at(i)->getCurInDegree()==0)
          {
            std::vector<Node*> *outputs=nodes->at(i)->getOutputs();
            std::vector<Node*>::iterator it;
            for(it=outputs->begin();it!=outputs->end();it++){
              Node* outNode=(*it);
              outNode->decCurInDegree();
            }
		
            sortedNodes.push_back(nodes->at(i));
            nodes->erase(nodes->begin()+i);
            break;
          }
      }
    }
  //write back to nodeList
  for (i=0;i<(int)sortedNodes.size();i++)
    {
      nodes->push_back(sortedNodes.at(i));
    }
  //init all nodes
  for(i=0;i<(int)nodes->size();i++)
    {
      nodes->at(i)->init();
    }
  //start all nodes
  for(i=0;i<(int)nodes->size();i++)
    {
      nodes->at(i)->start();
    }
}

void 
Manager::topologicalSortedTraversal(void* nodeList)
{
  if(!travBlock){
    travBlock=true;
    std::vector<Node *>* nodes=(std::vector<Node *>*)nodeList;

    // preprocess graph
    for(int i=0;i<(int)nodes->size();i++){
        //visit node i
        nodes->at(i)->preProcess();
    }
    // process graph
    for(int i=0;i<(int)nodes->size();i++){
      //visit node i
      nodes->at(i)->process();
    }
    // post process 
    for(int i=0;i<(int)nodes->size();i++){
      //visit node i
      nodes->at(i)->postProcess();
    }
    travBlock=false;
  }
}

void 
Manager::addNodeFactory(NodeFactory *aFactory)
{
  factories.push_back(aFactory);	
}



Node* 
Manager::getNode(std::string nodeName)
{
  for (int i=0;i<(int)nodes.size();i++){
    if(!strcmp(nodes[i]->getName(),nodeName.c_str()))
      {			
        return nodes[i];
        break;
      }
  }
    
  return NULL;
}



void 
Manager::initNodeFactories()
{
#ifdef ENABLE_VIDEOWRAPPERSRC
  factories.push_back(new VideoWrapperSrcFactory());
#endif

#ifdef ENABLE_GLUTSINK
  factories.push_back(new GLUTSinkFactory());
#endif

#ifdef ENABLE_GL_TEXTURE_2D_SINK
  factories.push_back(new GL_TEXTURE_2D_SinkFactory());
#endif

#ifdef ENABLE_TESTSRC
  factories.push_back(new TestSrcFactory());
#endif

#ifdef ENABLE_VIDEOSINK
  factories.push_back(new VideoSinkFactory());
#endif

#ifdef ENABLE_SPECTECSRC
  factories.push_back(new SpectecSrcFactory());
#endif

#ifdef ENABLE_OPENCV
  factories.push_back(new OpenCVSrcFactory());
#endif

}

