/* ======================================================================== 
* Copyright (C) 2006  Graz University of Technology  
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
* For further information please contact Dieter Schmalstieg under  
* <schmalstieg@icg.tu-graz.ac.at> or write to Dieter Schmalstieg,  
* Graz University of Technology, Inffeldgasse 16a, A8010 Graz,  
* Austria.  
* ========================================================================  
* PROJECT: OpenVideo  
* ======================================================================== */  
/** The header file for the Live555SrcFactory class.  
*  
* @author Bernhard Reitinger
*  
* $Id: Live555SrcFactory.h 25 2005-11-28 16:11:59Z breiting $  
* @file                                                                   */  
/* ======================================================================= */  

#ifndef _LIVE555SRCFACTORY_H_
#define _LIVE555SRCFACTORY_H_

#ifdef ENABLE_LIVE555SRC

#include <openvideo/openVideo.h>

#include <openvideo/NodeFactory.h>
#include <openvideo/nodes/Live555Src.h>

BEGIN_NAMESPACE_OV

/**
*	
*/
class Live555SrcFactory : 
        public openvideo::NodeFactory
{
public:
    /**
    *     The Constructor	
    */
    Live555SrcFactory();

    /**
    *     The destructor.
    */
    virtual ~Live555SrcFactory();

    /**
     *	creates nodes
     */
    virtual const char* getNodeTypeId();
	
    /**
     *	returns as the type of known objects
     */
    virtual openvideo::Live555Src* createNode();
protected:	

private:
	
};// class 

END_NAMESPACE_OV

#endif

#endif//_LIVE555SRC_H_

//========================================================================
// End of Live555SrcFactory.h 
//========================================================================
// Local Variables:
// mode: c++
// c-basic-offset: 4
// eval: (c-set-offset 'substatement-open 0)
// eval: (c-set-offset 'case-label '+)
// eval: (c-set-offset 'statement 'c-lineup-runin-statements)
// eval: (setq indent-tabs-mode nil)
// End:
//========================================================================
