/* Exception handling and frame unwind runtime interface routines. -*- C -*-
   Copyright (C) 2001-2017 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

/* This is derived from the C++ ABI for IA-64.  Where we diverge
   for cross-architecture compatibility are noted with "@@@".  
   This file is included from unwind-dw2.c, unwind-sjlj.c or
   unwind-ia64.c.  */

/* Subroutine of _Unwind_RaiseException also invoked from _Unwind_Resume. 

   Unwind the stack calling the personality routine to find both the
   exception handler and intermediary cleanup code.  We'll only locate
   the first such frame here.  Cleanup code will call back into
   _Unwind_Resume and we'll continue Phase 2 there.  */

static _Unwind_Reason_Code
_Unwind_RaiseException_Phase2(struct _Unwind_Exception *exc,
			      struct _Unwind_Context *context)
{
  _Unwind_Reason_Code code;

  while (1)
    {
      _Unwind_FrameState fs;
      int match_handler;

      code = uw_frame_state_for (context, &fs);

      /* Identify when we've reached the designated handler context.  */
      match_handler = (uw_identify_context (context) == exc->private_2
		       ? _UA_HANDLER_FRAME : 0);

      if (code != _URC_NO_REASON)
	/* Some error encountered.  Usually the unwinder doesn't
	   diagnose these and merely crashes.  */
	return _URC_FATAL_PHASE2_ERROR;

      /* Unwind successful.  Run the personality routine, if any.  */
      if (fs.personality)
	{
	  code = (*fs.personality) (1, _UA_CLEANUP_PHASE | match_handler,
				    exc->exception_class, exc, context);
	  if (code == _URC_INSTALL_CONTEXT)
	    break;
	  if (code != _URC_CONTINUE_UNWIND) 
	    return _URC_FATAL_PHASE2_ERROR;
	}

      /* Don't let us unwind past the handler context.  */
      assert (!match_handler);

      uw_update_context (context, &fs);
    }

  return code;
}

/* Subroutine of _Unwind_ForcedUnwind also invoked from _Unwind_Resume.  */

static _Unwind_Reason_Code
_Unwind_ForcedUnwind_Phase2 (struct _Unwind_Exception *exc,
			     struct _Unwind_Context *context)
{
  _Unwind_Stop_Fn stop = (_Unwind_Stop_Fn) (_Unwind_Ptr) exc->private_1;
  void *stop_argument = (void *) (_Unwind_Ptr) exc->private_2;
  _Unwind_Reason_Code code, stop_code;

  while (1)
    {
      _Unwind_FrameState fs;
      int action;

      /* Set up fs to describe the FDE for the caller of cur_context.  */
      code = uw_frame_state_for (context, &fs);
      if (code != _URC_NO_REASON && code != _URC_END_OF_STACK)
	return _URC_FATAL_PHASE2_ERROR;

      /* Unwind successful.  */
      action = _UA_FORCE_UNWIND | _UA_CLEANUP_PHASE;
      if (code == _URC_END_OF_STACK)
	action |= _UA_END_OF_STACK;
      stop_code = (*stop) (1, action, exc->exception_class, exc,
			   context, stop_argument);
      if (stop_code != _URC_NO_REASON)
	return _URC_FATAL_PHASE2_ERROR;

      /* Stop didn't want to do anything.  Invoke the personality
	 handler, if applicable, to run cleanups.  */
      if (code == _URC_END_OF_STACK)
	break;
	
      if (fs.personality)
	{
	  code = (*fs.personality) (1, _UA_FORCE_UNWIND | _UA_CLEANUP_PHASE,
				    exc->exception_class, exc, context);
	  if (code == _URC_INSTALL_CONTEXT)
	    break;
	  if (code != _URC_CONTINUE_UNWIND) 
	    return _URC_FATAL_PHASE2_ERROR;
	}

      /* Update cur_context to describe the same frame as fs, and discard
	 the previous context if necessary.  */
      uw_advance_context (context, &fs);
    }

  return code;
}



