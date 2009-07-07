/*******************************************************************************
FILE : time_editor.c

LAST MODIFIED : 17 May 2002

DESCRIPTION :
==============================================================================*/
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is cmgui.
 *
 * The Initial Developer of the Original Code is
 * Auckland Uniservices Ltd, Auckland, New Zealand.
 * Portions created by the Initial Developer are Copyright (C) 2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
#include <math.h>
#if defined (OLD_CODE)
/*???DB.  Why are these here? */
#define PI 3.1415927
#define PI_180 (PI/180.0)
#endif /* defined (OLD_CODE) */
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include "command/parser.h"
#include "general/callback_motif.h"
#include "general/debug.h"
#include "time/time.h"
#include "time/time_keeper.h"
#include "time/time_editor.h"
static char time_editor_uidh[] =
#include "time/time_editor.uidh"
	;
#include "user_interface/confirmation.h"
#include "user_interface/message.h"

/*
Module constants
----------------
*/
/* UIL Identifiers */
#define time_editor_play_ID           (1)
#define time_editor_play_reverse_ID   (2)
#define time_editor_stop_ID           (3)
#define time_editor_frame_text_ID     (4)
#define time_editor_step_ID           (5)
#define time_editor_step_back_ID      (6)
#define time_editor_two_step_ID       (7)
#define time_editor_two_step_back_ID  (8)
#define time_editor_set_framerate_ID  (9)
#define time_editor_set_step_ID      (10)
#define time_editor_play_every_ID    (11)
#define time_editor_close_ID         (12)
#define time_editor_settings_ID      (13)


/*
Module Types
------------
*/
struct Time_editor
/*******************************************************************************
LAST MODIFIED : 17 May 2002

DESCRIPTION :
Contains all the information carried by the time_editor widget.
Note that we just hold a pointer to the time_keeper, and must access and
deaccess it.
==============================================================================*/
{
	double step;
	struct Callback_data close_callback;
	struct Time_keeper *time_keeper;
	struct User_interface *user_interface;
	Widget frame_text,play_button,play_every_button,play_reverse_button,
		widget_parent,widget;
}; /* struct Time_editor */

struct Time_editor_defaults
{
	float step;
}; /* struct Time_editor_defaults */

/*
Module variables
----------------
*/
#if defined (MOTIF_USER_INTERFACE)
static int time_editor_hierarchy_open=0;
static MrmHierarchy time_editor_hierarchy;
#endif /* defined (MOTIF_USER_INTERFACE) */

/*
Module functions
----------------
*/

static int time_editor_time_keeper_callback(struct Time_keeper *time_keeper,
	enum Time_keeper_event event,void *time_editor_void)
/*******************************************************************************
LAST MODIFIED : 9 December 1998

DESCRIPTION :
Updates the time display of the text_widget
==============================================================================*/
{
	char time_string[60];
	double current_time;
	enum Time_keeper_play_direction play_direction;
	int return_code;
	struct Time_editor *time_editor;

	ENTER(time_editor_destroy_CB);
	if (time_keeper&&(time_editor=(struct Time_editor *)time_editor_void))
	{
		switch (event)
		{
			case TIME_KEEPER_NEW_TIME:
			{
				current_time=Time_keeper_get_time(time_editor->time_keeper);
				sprintf(time_string,"%10.6g",current_time);
#if defined (DEBUG)
				/*???debug */
				printf("time_editor_time_keeper_callback.  time %10.6g\n",current_time);
#endif /* defined (DEBUG) */
				XtVaSetValues(time_editor->frame_text,
					XmNvalue,time_string,
					NULL);
				return_code=1;
			} break;
			case TIME_KEEPER_STOPPED:
			{
				if (time_editor->play_button)
				{
					XmToggleButtonSetState(time_editor->play_button,False,False);
				}
				if (time_editor->play_reverse_button)
				{
					XmToggleButtonSetState(time_editor->play_reverse_button,False,False);
				}
			} break;
			case TIME_KEEPER_STARTED:
			case TIME_KEEPER_CHANGED_DIRECTION:
			{
				play_direction=Time_keeper_get_play_direction(
					time_editor->time_keeper);
				switch (play_direction)
				{
					case TIME_KEEPER_PLAY_FORWARD:
					{
						if (time_editor->play_button)
						{
							XmToggleButtonSetState(time_editor->play_button,True,False);
						}
						if (time_editor->play_reverse_button)
						{
							XmToggleButtonSetState(time_editor->play_reverse_button,False,
								False);
						}
					} break;
					case TIME_KEEPER_PLAY_BACKWARD:
					{
						if (time_editor->play_button)
						{
							XmToggleButtonSetState(time_editor->play_button,False,False);
						}
						if (time_editor->play_reverse_button)
						{
							XmToggleButtonSetState(time_editor->play_reverse_button,True,
								False);
						}
					} break;
					default:
					{
						display_message(ERROR_MESSAGE,
							"time_editor_time_keeper_callback.  Unknown play direction");
						return_code=0;
					} break;
				}
			} break;
			default:
			{
				display_message(ERROR_MESSAGE,
					"time_editor_time_keeper_callback.  Unknown time_keeper event");
				return_code=0;
			} break;
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"time_editor_time_keeper_callback.  Invalid argument(s)");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* time_editor_time_keeper_callback */

static void time_editor_identify_widget(Widget widget,int widget_num,
	unsigned long *reason)
/*******************************************************************************
LAST MODIFIED : 24 November 1999

DESCRIPTION :
Finds the id of the widgets on the time_editor widget.
==============================================================================*/
{
	struct Time_editor *time_editor;

	ENTER(time_editor_identify_widget);
	USE_PARAMETER(reason);
	/* find out which time_editor widget we are in */
	XtVaGetValues(widget,XmNuserData,&time_editor,NULL);
	switch (widget_num)
	{
		case time_editor_frame_text_ID:
		{
			time_editor->frame_text=widget;
		} break;
		case time_editor_play_ID:
		{
			time_editor->play_button=widget;
		} break;
		case time_editor_play_reverse_ID:
		{
			time_editor->play_reverse_button=widget;
		} break;
		case time_editor_play_every_ID:
		{
			time_editor->play_every_button=widget;
		} break;
		default:
		{
			display_message(WARNING_MESSAGE,
				"time_editor_identify_widget.  Invalid widget number");
		} break;
	}
	LEAVE;
} /* time_editor_identify_widget */

static void time_editor_control_CB(Widget widget,int widget_num,
	unsigned long *reason)
/*******************************************************************************
LAST MODIFIED : 24 November 1999

DESCRIPTION :
==============================================================================*/
{
	char *text_string,default_string[30];
	double time;
	struct Time_keeper *time_keeper;
	struct Time_editor *time_editor;

	ENTER(time_editor_control_CB);
	USE_PARAMETER(reason);
	if (widget)
	{
		XtVaGetValues(widget,
			XmNuserData,&time_editor,
			NULL);

		if (time_editor && (time_keeper=time_editor->time_keeper))
		{
			switch (widget_num)
			{
				case time_editor_frame_text_ID:
				{
					text_string=XmTextFieldGetString(widget);
					if (sscanf(text_string,"%lf",&time))
					{
						if (time!=Time_keeper_get_time(time_keeper))
						{
							Time_keeper_request_new_time(time_keeper,time);
						}
					}
					else
					{
						/* Get the text widget to display the correct time */
						time_editor_time_keeper_callback(time_keeper,
							TIME_KEEPER_NEW_TIME,(void *)time_editor);
					}
					XtFree(text_string);
				} break;
				case time_editor_play_ID:
				{
					if (XmToggleButtonGetState(widget))
					{
						Time_keeper_play(time_keeper,TIME_KEEPER_PLAY_FORWARD);
					}
					else
					{
						Time_keeper_stop(time_keeper);
					}
				} break;
				case time_editor_play_reverse_ID:
				{
					if (XmToggleButtonGetState(widget))
					{
						Time_keeper_play(time_keeper,TIME_KEEPER_PLAY_BACKWARD);
					}
					else
					{
						Time_keeper_stop(time_keeper);
					}
				} break;
				case time_editor_stop_ID:
				{
					Time_keeper_stop(time_keeper);
				} break;
				case time_editor_step_ID:
				{
					time=Time_keeper_get_time(time_keeper);
					Time_keeper_request_new_time(time_keeper,
						time + time_editor->step);
				} break;
				case time_editor_step_back_ID:
				{
					time=Time_keeper_get_time(time_keeper);
					Time_keeper_request_new_time(time_keeper,
						time - time_editor->step);
				} break;
				case time_editor_two_step_ID:
				{
					time=Time_keeper_get_time(time_keeper);
					Time_keeper_request_new_time(time_keeper,
						time + 2.0 * time_editor->step);
				} break;
				case time_editor_two_step_back_ID:
				{
					time=Time_keeper_get_time(time_keeper);
					Time_keeper_request_new_time(time_keeper,
						time - 2.0 * time_editor->step);
				} break;
				case time_editor_set_framerate_ID:
				{
					time=Time_keeper_get_speed(time_keeper);
					sprintf(default_string,"%10.6g",time);
					if (text_string=confirmation_get_string("Set framerate",
						"Enter the new framerate",default_string,time_editor->widget,
						time_editor->user_interface))
					{
						if (sscanf(text_string,"%lf",&time))
						{
							Time_keeper_set_speed(time_keeper,time);
						}
						DEALLOCATE(text_string);
					}
				} break;
				case time_editor_set_step_ID:
				{
					sprintf(default_string,"%10.6g",time_editor->step);
					if (text_string=confirmation_get_string("Set step",
						"Enter the new step size",default_string,time_editor->widget,
						time_editor->user_interface))
					{
						if (sscanf(text_string,"%lf",&time))
						{
							time_editor->step=time;
						}
						DEALLOCATE(text_string);
					}
				} break;
				case time_editor_play_every_ID:
				{
					if (XmToggleButtonGetState(widget))
					{
						Time_keeper_set_play_every_frame(time_keeper);
					}
					else
					{
						Time_keeper_set_play_skip_frames(time_keeper);
					}
				} break;
				case time_editor_close_ID:
				{
					if (time_editor->close_callback.procedure)
					{
						(time_editor->close_callback.procedure)
							(time_editor->widget, time_editor->close_callback.data,
							time_editor->time_keeper);
					}
					XtDestroyWidget(XtParent(XtParent(time_editor->widget_parent)));
				} break;
				case time_editor_settings_ID:
				{
					if (Time_keeper_get_play_every_frame(time_keeper))
					{
						XmToggleButtonSetState(time_editor->play_every_button,True,
							False);
					}
					else
					{
						XmToggleButtonSetState(time_editor->play_every_button,False,
							False);
					}
				} break;
				default:
				{
					display_message(WARNING_MESSAGE,
						"time_editor_control_CB.  Invalid widget number");
				} break;
			}
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"time_editor_control_CB.  Missing time_editor or time_keeper");
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"time_editor_control_CB.  Missing widget");
	}
	LEAVE;
} /* time_editor_control_CB */

/*
Global functions
----------------
*/
struct Time_editor *CREATE(Time_editor)(
	struct Time_editor **time_editor_address,
	Widget parent,struct Time_keeper *time_keeper,
	struct User_interface *user_interface)
/*******************************************************************************
LAST MODIFIED : 17 May 2002

DESCRIPTION :
Creates a <*time_editor_address>.
==============================================================================*/
{
	int init_widgets;
	MrmType time_editor_dialog_class;
	struct Time_editor *time_editor;
#define XmNtimeEditorStep "timeEditorStep"
#define XmCtimeEditorStep "TimeEditorStep"
	struct Time_editor_defaults time_editor_defaults;
	static XtResource resources[]=
	{
		{
			XmNtimeEditorStep,
			XmCtimeEditorStep,
			XmRFloat,
			sizeof(float),
			XtOffsetOf(struct Time_editor_defaults,step),
			XmRString,
			"1.0"
		}
	};
	static MrmRegisterArg callback_list[]=
	{
		{"time_editor_identify_widget",(XtPointer)time_editor_identify_widget},
		{"time_editor_control_CB",(XtPointer)time_editor_control_CB},
	};
	static MrmRegisterArg identifier_list[]=
	{
		{"time_editor_structure",(XtPointer)NULL},
		{"time_editor_play_ID",(XtPointer)time_editor_play_ID},
		{"time_editor_play_reverse_ID",(XtPointer)time_editor_play_reverse_ID},
		{"time_editor_stop_ID",(XtPointer)time_editor_stop_ID},
		{"time_editor_frame_text_ID",(XtPointer)time_editor_frame_text_ID},
		{"time_editor_step_ID",(XtPointer)time_editor_step_ID},
		{"time_editor_step_back_ID",(XtPointer)time_editor_step_back_ID},
		{"time_editor_two_step_ID",(XtPointer)time_editor_two_step_ID},
		{"time_editor_two_step_back_ID",(XtPointer)time_editor_two_step_back_ID},
		{"time_editor_set_framerate_ID",(XtPointer)time_editor_set_framerate_ID},
		{"time_editor_set_step_ID",(XtPointer)time_editor_set_step_ID},
		{"time_editor_play_every_ID",(XtPointer)time_editor_play_every_ID},
		{"time_editor_close_ID",(XtPointer)time_editor_close_ID},
		{"time_editor_settings_ID",(XtPointer)time_editor_settings_ID}
	};

	ENTER(CREATE(Time_editor));
	time_editor=(struct Time_editor *)NULL;
	if (time_editor_address&&parent&&user_interface)
	{
		if (MrmOpenHierarchy_binary_string(time_editor_uidh,sizeof(time_editor_uidh),
			&time_editor_hierarchy,&time_editor_hierarchy_open))
		{
			/* allocate memory */
			if (ALLOCATE(time_editor,struct Time_editor,1))
			{
				/* initialise the structure */
				/* time_editor->manager_callback_id=(void *)NULL; */
				time_editor_defaults.step = 1.0;
				XtVaGetApplicationResources(User_interface_get_application_shell(user_interface),
					&time_editor_defaults,resources,XtNumber(resources),NULL);
				time_editor->step=time_editor_defaults.step;
				time_editor->frame_text=(Widget)NULL;
				time_editor->play_button=(Widget)NULL;
				time_editor->play_reverse_button=(Widget)NULL;
				time_editor->play_every_button=(Widget)NULL;
				time_editor->widget_parent=parent;
				time_editor->widget=(Widget)NULL;
				time_editor->time_keeper=(struct Time_keeper *)NULL;
				time_editor->close_callback.procedure=(Callback_procedure *)NULL;
				time_editor->close_callback.data=NULL;
				time_editor->user_interface=user_interface;
				/* register the callbacks */
				if (MrmSUCCESS==MrmRegisterNamesInHierarchy(time_editor_hierarchy,
					callback_list,XtNumber(callback_list)))
				{
					/* assign and register the identifiers */
					identifier_list[0].value=(XtPointer)time_editor;
					if (MrmSUCCESS==MrmRegisterNamesInHierarchy(time_editor_hierarchy,
						identifier_list,XtNumber(identifier_list)))
					{
						/* fetch position window widget */
						if (MrmSUCCESS==MrmFetchWidget(time_editor_hierarchy,
							"time_editor_widget",time_editor->widget_parent,
							&(time_editor->widget),&time_editor_dialog_class))
						{
							/* leave managing up to time_editor_set_time */
							/* ie. as soon as we have a valid time */
							/* XtManageChild(time_editor->widget); */
							/* set the mode toggle to the correct position */
							init_widgets=1;
							/* Initialise sub widgets here */
							if (init_widgets)
							{
								install_accelerators(time_editor->widget,time_editor->widget);
								if (time_keeper)
								{
									time_editor_set_time_keeper(time_editor,time_keeper);
								}
								*time_editor_address=time_editor;
							}
							else
							{
								DEALLOCATE(time_editor);
							}
						}
						else
						{
							display_message(ERROR_MESSAGE,"CREATE(Time_editor).  "
								"Could not fetch time_editor dialog");
							DEALLOCATE(time_editor);
						}
					}
					else
					{
						display_message(ERROR_MESSAGE,
							"CREATE(Time_editor).  Could not register identifiers");
						DEALLOCATE(time_editor);
					}
				}
				else
				{
					display_message(ERROR_MESSAGE,
						"CREATE(Time_editor).  Could not register callbacks");
					DEALLOCATE(time_editor);
				}
			}
			else
			{
				display_message(ERROR_MESSAGE,"CREATE(Time_editor).  "
					"Could not allocate time_editor widget structure");
			}
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"CREATE(Time_editor).  Could not open hierarchy");
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"CREATE(Time_editor).  Invalid argument(s)");
	}
	LEAVE;

	return (time_editor);
} /* CREATE(Time_editor) */

int DESTROY(Time_editor)(struct Time_editor **time_editor_address)
/*******************************************************************************
LAST MODIFIED : 17 May 2002

DESCRIPTION :
Destroy the <*time_editor_address> and sets <*time_editor_address> to NULL.
==============================================================================*/
{
	int return_code;
	struct Time_editor *time_editor;

	ENTER(DESTROY(Time_editor));
	return_code=0;
	if (time_editor_address&&(time_editor= *time_editor_address))
	{
		if (time_editor->time_keeper)
		{
			Time_keeper_remove_callback(time_editor->time_keeper,
				time_editor_time_keeper_callback,(void *)time_editor);
			DEACCESS(Time_keeper)(&(time_editor->time_keeper));
		}
		DEALLOCATE(time_editor);
		*time_editor_address=(struct Time_editor *)NULL;
		return_code=1;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"DESTROY(Time_editor).  Invalid argument(s)");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* DESTROY(Time_editor) */

int time_editor_get_close_callback(struct Time_editor *time_editor,
	struct Callback_data *callback)
/*******************************************************************************
LAST MODIFIED : 8 June 2004

DESCRIPTION :
Get the close <callback> information for the <time_editor>.
==============================================================================*/
{
	int return_code;

	ENTER(time_editor_get_callback);
	return_code=0;
	if (time_editor&&callback)
	{
		callback->procedure=time_editor->close_callback.procedure;
		callback->data=time_editor->close_callback.data;
		return_code=1;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"time_editor_get_close_callback.  Invalid argument(s)");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* time_editor_get_close_callback */

int time_editor_set_close_callback(struct Time_editor *time_editor,
	struct Callback_data *callback)
/*******************************************************************************
LAST MODIFIED : 8 June 2004

DESCRIPTION :
Set the close <callback> information for the <time_editor>.
==============================================================================*/
{
	int return_code;

	ENTER(time_editor_set_callback);
	return_code=0;
	if (time_editor&&callback)
	{
		time_editor->close_callback.procedure=callback->procedure;
		time_editor->close_callback.data=callback->data;
		return_code=1;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"time_editor_set_close_callback.  Invalid argument(s)");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* time_editor_set_close_callback */

int time_editor_get_time_keeper(struct Time_editor *time_editor,
	struct Time_keeper **time_keeper_address)
/*******************************************************************************
LAST MODIFIED : 17 May 2002

DESCRIPTION :
Get the <*time_keeper_address> for the <time_editor>.
Do not modify or DEALLOCATE the returned time_keeper.
==============================================================================*/
{
	int return_code;

	ENTER(time_editor_set_time);
	return_code=0;
	/* check arguments */
	if (time_editor&&time_keeper_address)
	{
		*time_keeper_address=time_editor->time_keeper;
		return_code=1;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"time_editor_get_time_keeper.  Invalid argument(s)");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* time_editor_get_time_keeper */

int time_editor_set_time_keeper(struct Time_editor *time_editor,
	struct Time_keeper *time_keeper)
/*******************************************************************************
LAST MODIFIED : 17 May 2002

DESCRIPTION :
Set the <time_keeper> for the <time_editor>.
==============================================================================*/
{
	int return_code;

	ENTER(time_editor_set_time_keeper);
	return_code=0;
	/* check arguments */
	if (time_editor)
	{
		return_code=1;
		if (time_keeper!=time_editor->time_keeper)
		{
			if (time_editor->time_keeper)
			{
				Time_keeper_remove_callback(time_editor->time_keeper,
					time_editor_time_keeper_callback,(void *)time_editor);
				DEACCESS(Time_keeper)(&(time_editor->time_keeper));
			}
			if (time_keeper)
			{
				time_editor->time_keeper=ACCESS(Time_keeper)(time_keeper);
				Time_keeper_add_callback(time_editor->time_keeper,
					time_editor_time_keeper_callback,(void *)time_editor,
					(enum Time_keeper_event)(TIME_KEEPER_NEW_TIME|TIME_KEEPER_STARTED|
					TIME_KEEPER_STOPPED|TIME_KEEPER_CHANGED_DIRECTION));
				/* Get the text widget to display the correct time */
				time_editor_time_keeper_callback(time_keeper,
					TIME_KEEPER_NEW_TIME,(void *)time_editor);
				if (Time_keeper_is_playing(time_keeper))
				{
					time_editor_time_keeper_callback(time_keeper,
						TIME_KEEPER_STARTED,(void *)time_editor);
				}
				else
				{
					time_editor_time_keeper_callback(time_keeper,
						TIME_KEEPER_STOPPED,(void *)time_editor);
				}
				XtManageChild(time_editor->widget);
			}
			else
			{
				time_editor->time_keeper=(struct Time_keeper *)NULL;
				XtUnmanageChild(time_editor->widget);
			}
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"time_editor_set_time_keeper.  Invalid argument(s).");
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* time_editor_set_time_keeper */

int time_editor_get_step(struct Time_editor *time_editor,float *step)
/*******************************************************************************
LAST MODIFIED : 17 May 2002

DESCRIPTION :
Get the <*step> for the <time_editor>.
==============================================================================*/
{
	int return_code;

	ENTER(time_editor_get_step);
	return_code=0;
	/* check arguments */
	if (time_editor&&step)
	{
		*step=(float)(time_editor->step);
		return_code=1;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"time_editor_get_step.  Invalid argument(s).  %p %p",time_editor,step);
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* time_editor_get_step */

int time_editor_set_step(struct Time_editor *time_editor,float step)
/*******************************************************************************
LAST MODIFIED : 17 May 2002

DESCRIPTION :
Set the <step> for the <time_editor>.
==============================================================================*/
{
	int return_code;

	ENTER(time_editor_set_step);
	return_code=0;
	/* check arguments */
	if (time_editor&&(step>0))
	{
		time_editor->step=(float)step;
		return_code=1;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"time_editor_set_step.  Invalid argument(s).  %p %p",time_editor,step);
		return_code=0;
	}
	LEAVE;

	return (return_code);
} /* time_editor_set_step */
