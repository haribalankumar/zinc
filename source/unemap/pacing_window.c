/*******************************************************************************
FILE : pacing_window.c

LAST MODIFIED : 11 November 2001

DESCRIPTION :
==============================================================================*/
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#if defined (MOTIF)
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xlib.h>
#include <X11/Composite.h>
#include <Xm/Xm.h>
#include <Xm/PushBG.h>
#include <Mrm/MrmPublic.h>
#include <Mrm/MrmDecls.h>
#endif /* defined (MOTIF) */
#include "unemap/pacing_window.h"
#if defined (MOTIF)
#include "unemap/pacing_window.uidh"
#endif /* defined (MOTIF) */
#include "general/debug.h"
#include "general/mystring.h"
#include "unemap/unemap_hardware.h"
#include "user_interface/message.h"
#include "user_interface/user_interface.h"

/*
Module types
------------
*/
struct Pacing_window
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
The window associated with the set up button.
==============================================================================*/
{
	char *decrement_threshold_pairs_string,pacing,*pacing_electrodes,response,
		*return_electrodes;
	float basic_cycle_length,control_voltage,*decrement_threshold_pairs,
		*pacing_voltages,s2_delay,s2_delay_change,s2_delay_change_factor,
		s2_resolution,s2_s1_pause;
	int number_of_pacing_channels,number_of_return_channels,number_of_s1,
		number_of_decrement_threshold_pairs,*pacing_channels,*return_channels,
		total_number_of_s1_pacing_voltages;
	struct Pacing_window **address;
	struct Rig **rig_address;
	struct User_interface *user_interface;
#if defined (MOTIF)
	Widget activation,window,shell;
	Widget basic_cycle_length_form,basic_cycle_length_value;
	Widget control_voltage_form,control_voltage_value;
	Widget decrement_threshold_pairs_form,decrement_threshold_pairs_value;
	Widget number_of_s1_form,number_of_s1_value;
	Widget pacing_electrodes_form,pacing_electrodes_value;
	Widget return_electrodes_form,return_electrodes_value;
	Widget s2_delay_form,s2_delay_value;
	Widget s2_delay_change_form,s2_delay_change_value;
	Widget s2_delay_change_factor_form,s2_delay_change_factor_value;
	Widget s2_resolution_form,s2_resolution_value;
	Widget s2_s1_pause_form,s2_s1_pause_value;
	Widget basic_cycle_length_pace_toggle,restitution_curve_pace_toggle,
		restitution_time_pace_toggle,restitution_time_no_button,
		restitution_time_yes_button;
#endif /* defined (MOTIF) */
}; /* struct Pacing_window */

typedef struct Pacing_window Pacing_window_settings;

/*
Module variables
----------------
*/
#if defined (MOTIF)
static int pacing_window_hierarchy_open=0;
static MrmHierarchy pacing_window_hierarchy;
#endif /* defined (MOTIF) */

/*
Module functions
----------------
*/
static int set_channels_from_string(char *electrodes,struct Rig *rig,
	int number_of_restricted_channels,int *restricted_channels,
	char **electrodes_result_address,int *number_of_channels_address,
	int **channels_address)
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
Parses the <electrodes> string to get the channels for the <rig>.
==============================================================================*/
{
	char *electrodes_result,*electrodes_result_temp;
	int channel_number,*channels,*channels_temp,electrodes_result_length,found,i,
		j,number_of_channels,number_of_characters,number_of_devices,
		number_of_stimulators,position,*restricted_stimulators,return_code;
	struct Device **device;

	ENTER(set_channels_from_string);
	return_code=0;
	/* check arguments */
	if (electrodes&&rig&&electrodes_result_address&&number_of_channels_address&&
		channels_address)
	{
		return_code=1;
		DEALLOCATE(*electrodes_result_address);
		DEALLOCATE(*channels_address);
		*number_of_channels_address=0;
		if ((0<number_of_restricted_channels)&&restricted_channels&&
			unemap_get_number_of_stimulators(&number_of_stimulators)&&
			(0<number_of_stimulators)&&ALLOCATE(restricted_stimulators,int,
			number_of_stimulators))
		{
			for (i=0;i<number_of_stimulators;i++)
			{
				restricted_stimulators[i]=0;
			}
			for (i=0;i<number_of_restricted_channels;i++)
			{
				j=number_of_stimulators;
				while ((j>0)&&
					!unemap_channel_valid_for_stimulator(j,restricted_channels[i]))
				{
					j--;
				}
				if (j>0)
				{
					restricted_stimulators[j-1]=1;
				}
			}
		}
		else
		{
			number_of_stimulators=0;
			restricted_stimulators=(int *)NULL;
		}
		channels=(int *)NULL;
		number_of_channels=0;
		electrodes_result=(char *)NULL;
		electrodes_result_length=0;
		position=0;
		number_of_characters=0;
		sscanf(electrodes+position,"%*[ ,]%n",&number_of_characters);
		if (0<number_of_characters)
		{
			position=number_of_characters;
		}
		while (return_code&&electrodes[position])
		{
			number_of_characters=0;
			sscanf(electrodes+position,"%*[^ ,]%n",&number_of_characters);
			if (0<number_of_characters)
			{
				device=rig->devices;
				number_of_devices=rig->number_of_devices;
				found=0;
				while (!found&&(0<number_of_devices))
				{
					if ((ELECTRODE==(*device)->description->type)&&
						(number_of_characters==strlen((*device)->description->name))&&
						(0==strncmp(electrodes+position,(*device)->description->name,
						number_of_characters)))
					{
						found=1;
					}
					else
					{
						device++;
						number_of_devices--;
					}
				}
				if (found)
				{
					/* check that don't already have channel */
					i=0;
					while ((i<number_of_channels)&&(channels[i]!=
						(*device)->channel->number))
					{
						i++;
					}
					if (i>=number_of_channels)
					{
						channel_number=(*device)->channel->number;
						/* check that not restricted */
						if (restricted_stimulators)
						{
							i=0;
							while ((i<number_of_stimulators)&&!(restricted_stimulators[i]&&
								unemap_channel_valid_for_stimulator(i+1,channel_number)))
							{
								i++;
							}
						}
						else
						{
							i=0;
						}
						if (i==number_of_stimulators)
						{
							if (REALLOCATE(channels_temp,channels,int,number_of_channels+1)&&
								REALLOCATE(electrodes_result_temp,electrodes_result,char,
								electrodes_result_length+number_of_characters+2))
							{
								channels=channels_temp;
								channels[number_of_channels]=channel_number;
								number_of_channels++;
								electrodes_result=electrodes_result_temp;
								if (number_of_channels>1)
								{
									electrodes_result[electrodes_result_length]=',';
									electrodes_result_length++;
								}
								strncpy(electrodes_result+electrodes_result_length,
									electrodes+position,number_of_characters);
								electrodes_result_length += number_of_characters;
								electrodes_result[electrodes_result_length]='\0';
							}
							else
							{
								if (channels_temp)
								{
									channels=channels_temp;
								}
								return_code=0;
							}
						}
					}
				}
				position += number_of_characters;
				number_of_characters=0;
				sscanf(electrodes+position,"%*[ ,]%n",&number_of_characters);
				if (0<number_of_characters)
				{
					position += number_of_characters;
				}
			}
			else
			{
				return_code=0;
			}
		}
		DEALLOCATE(restricted_stimulators);
		*number_of_channels_address=number_of_channels;
		*channels_address=channels;
		*electrodes_result_address=electrodes_result;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"set_channels_from_string.  Invalid argument(s).  %p %p %p %p %p",
			electrodes,rig,electrodes_result_address,number_of_channels_address,
			channels_address);
	}
	LEAVE;

	return (return_code);
} /* set_channels_from_string */

static int set_pacing_channels_from_string(struct Pacing_window *pacing_window,
	char *pacing_electrodes)
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
Parses the <pacing_electrodes> string to get the pacing channels for the
<pacing_window>.
==============================================================================*/
{
	int return_code;

	ENTER(set_pacing_channels_from_string);
	return_code=0;
	/* check arguments */
	if (pacing_window&&pacing_electrodes&&(pacing_window->rig_address))
	{
		return_code=set_channels_from_string(pacing_electrodes,
			*(pacing_window->rig_address),pacing_window->number_of_return_channels,
			pacing_window->return_channels,&(pacing_window->pacing_electrodes),
			&(pacing_window->number_of_pacing_channels),
			&(pacing_window->pacing_channels));
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"set_pacing_channels_from_string.  Invalid argument(s).  %p %p",
			pacing_window,pacing_electrodes);
	}
	LEAVE;

	return (return_code);
} /* set_pacing_channels_from_string */

static int set_return_channels_from_string(struct Pacing_window *pacing_window,
	char *return_electrodes)
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
Parses the <return_electrodes> string to get the return channels for the
<pacing_window>.
==============================================================================*/
{
	int return_code;

	ENTER(set_return_channels_from_string);
	return_code=0;
	/* check arguments */
	if (pacing_window&&return_electrodes&&(pacing_window->rig_address))
	{
		return_code=set_channels_from_string(return_electrodes,
			*(pacing_window->rig_address),pacing_window->number_of_pacing_channels,
			pacing_window->pacing_channels,&(pacing_window->return_electrodes),
			&(pacing_window->number_of_return_channels),
			&(pacing_window->return_channels));
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"set_return_channels_from_string.  Invalid argument(s).  %p %p",
			pacing_window,return_electrodes);
	}
	LEAVE;

	return (return_code);
} /* set_return_channels_from_string */

static int set_decrement_threshold_pairs_from_string(
	struct Pacing_window *pacing_window,char *decrement_threshold_pairs_string)
/*******************************************************************************
LAST MODIFIED : 4 November 2001

DESCRIPTION :
Parses the <decrement_threshold_pairs_string> to get the steps to take for the
restitution curve protocol.
==============================================================================*/
{
	char *temp_decrement_threshold_pairs_string,temp_string[101];
	float decrement,*decrement_threshold_pairs,*temp_decrement_threshold_pairs,
		threshold;
	int i,j,number_of_characters,number_of_decrement_threshold_pairs,position,
		return_code;

	ENTER(set_decrement_threshold_pairs_from_string);
	return_code=0;
	/* check arguments */
	if (pacing_window&&decrement_threshold_pairs)
	{
		return_code=1;
		DEALLOCATE(pacing_window->decrement_threshold_pairs_string);
		assign_empty_string(&(pacing_window->decrement_threshold_pairs_string));
		DEALLOCATE(pacing_window->decrement_threshold_pairs);
		pacing_window->number_of_decrement_threshold_pairs=0;
		number_of_decrement_threshold_pairs=0;
		decrement_threshold_pairs=(float *)NULL;
		position=0;
		/* skip separators */
		number_of_characters=0;
		sscanf(decrement_threshold_pairs_string+position,"%*[ ,;()]%n",
			&number_of_characters);
		if (0<number_of_characters)
		{
			position=number_of_characters;
		}
		while (return_code&&decrement_threshold_pairs_string[position])
		{
			/* read decrement */
			number_of_characters=0;
			sscanf(decrement_threshold_pairs_string+position,"%*[^ ,;()]%n",
				&number_of_characters);
			if ((0<number_of_characters)&&
				(1==sscanf(decrement_threshold_pairs_string+position,"%f",&decrement)))
			{
				position += number_of_characters;
				/* must be a multiple of the S2 resolution */
				if (decrement<pacing_window->s2_resolution)
				{
					decrement=pacing_window->s2_resolution;
				}
				else
				{
					decrement=(pacing_window->s2_resolution)*
						floor(decrement/(pacing_window->s2_resolution)+0.5);
				}
				/* skip separators */
				number_of_characters=0;
				sscanf(decrement_threshold_pairs_string+position,"%*[ ,;()]%n",
					&number_of_characters);
				if (0<number_of_characters)
				{
					position += number_of_characters;
				}
				if (decrement_threshold_pairs_string[position])
				{
					/* read threshold */
					number_of_characters=0;
					sscanf(decrement_threshold_pairs_string+position,"%*[^ ,;()]%n",
						&number_of_characters);
					if ((0<number_of_characters)&&(1==sscanf(
						decrement_threshold_pairs_string+position,"%f",&threshold)))
					{
						position += number_of_characters;
						/* must be a multiple of the S2 resolution */
						if (threshold<pacing_window->s2_resolution)
						{
							threshold=pacing_window->s2_resolution;
						}
						else
						{
							threshold=(pacing_window->s2_resolution)*
								floor(threshold/(pacing_window->s2_resolution)+0.5);
						}
						/* add to array */
						i=0;
						while ((i<number_of_decrement_threshold_pairs)&&
							(decrement_threshold_pairs[2*i+1]>threshold))
						{
							i++;
						}
						if ((i<number_of_decrement_threshold_pairs)&&
							(decrement_threshold_pairs[2*i+1]==threshold))
						{
							/* threshold already in list.  Use the new decrement */
							decrement_threshold_pairs[2*i]=decrement;
						}
						else
						{
							/* new pair */
							if (REALLOCATE(temp_decrement_threshold_pairs,
								decrement_threshold_pairs,float,
								2*number_of_decrement_threshold_pairs+2))
							{
								decrement_threshold_pairs=temp_decrement_threshold_pairs;
								/* make room for the new pair */
								j=number_of_decrement_threshold_pairs;
								while (j>i)
								{
									decrement_threshold_pairs[2*j+1]=
										decrement_threshold_pairs[2*j-1];
									decrement_threshold_pairs[2*j]=
									decrement_threshold_pairs[2*j-2];
									j--;
								}
								decrement_threshold_pairs[2*i+1]=threshold;
								decrement_threshold_pairs[2*i]=decrement;
								number_of_decrement_threshold_pairs++;
							}
							else
							{
								return_code=0;
							}
						}
						/* skip separators */
						number_of_characters=0;
						sscanf(decrement_threshold_pairs_string+position,"%*[ ,;()]%n",
							&number_of_characters);
						if (0<number_of_characters)
						{
							position += number_of_characters;
						}
					}
					else
					{
						return_code=0;
					}
				}
				else
				{
					return_code=0;
				}
			}
			else
			{
				return_code=0;
			}
		}
		return_code=1;
		i=0;
		while (return_code&&(i<number_of_decrement_threshold_pairs))
		{
			sprintf(temp_string,"%g,%g;",decrement_threshold_pairs[2*i],
				decrement_threshold_pairs[2*i+1]);
			if (REALLOCATE(temp_decrement_threshold_pairs_string,
				pacing_window->decrement_threshold_pairs_string,char,
				strlen(pacing_window->decrement_threshold_pairs_string)+
				strlen(decrement_threshold_pairs_string)))
			{
				strcat(temp_decrement_threshold_pairs_string,temp_string);
				pacing_window->decrement_threshold_pairs_string=
					temp_decrement_threshold_pairs_string;
				i++;
			}
			else
			{
				return_code=0;
			}
		}
		pacing_window->number_of_decrement_threshold_pairs=i;
		pacing_window->decrement_threshold_pairs=decrement_threshold_pairs;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"set_decrement_threshold_pairs_from_string.  Invalid argument(s).  %p %p",
			pacing_window,decrement_threshold_pairs_string);
	}
	LEAVE;

	return (return_code);
} /* set_decrement_threshold_pairs_from_string */

#if defined (MOTIF)
static void destroy_Pacing_window_callback(Widget widget,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 10 October 2001

DESCRIPTION :
Tidys up when the user destroys the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(destroy_Pacing_window_callback);
	USE_PARAMETER(widget);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		destroy_Pacing_window(&pacing_window);
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"destroy_Pacing_window_callback.  Missing pacing_window_structure");
	}
	LEAVE;
} /* destroy_Pacing_window_callback */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_electrodes_form(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 15 October 2001

DESCRIPTION :
Finds the id of the pacing electrodes form in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_electrodes_form);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->pacing_electrodes_form= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_electrodes_form.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_electrodes_form */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_electrodes_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 15 October 2001

DESCRIPTION :
Finds the id of the pacing electrodes value in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_electrodes_value);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->pacing_electrodes_value= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_electrodes_value.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_electrodes_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ch_pacing_electrodes_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 15 October 2001

DESCRIPTION :
Called when the pacing electrodes widget is changed.
==============================================================================*/
{
	char *new_value,*pacing_electrodes;
	struct Pacing_window *pacing_window;
	XmAnyCallbackStruct *text_data

	ENTER(ch_pacing_electrodes_value);
	USE_PARAMETER(widget_id);
	if ((text_data=(XmAnyCallbackStruct *)call_data)&&
		((XmCR_ACTIVATE==text_data->reason)||
		(XmCR_LOSING_FOCUS==text_data->reason)))
	{
		if (pacing_window=(struct Pacing_window *)pacing_window_structure)
		{
			if (!(pacing_window->pacing))
			{
				XtVaGetValues(pacing_window->pacing_electrodes_value,
					XmNvalue,&new_value,
					NULL);
				set_pacing_channels_from_string(pacing_window,new_value);
			}
			if (!(pacing_electrodes=pacing_window->pacing_electrodes))
			{
				pacing_electrodes="";
			}
			XtVaSetValues(pacing_window->pacing_electrodes_value,
				XmNcursorPosition,strlen(pacing_electrodes),
				XmNvalue,pacing_electrodes,
				NULL);
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"ch_pacing_electrodes_value.  Missing pacing_window_structure");
		}
	}
	LEAVE;
} /* ch_pacing_electrodes_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_return_electrodes_for(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
Finds the id of the return electrodes form in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_return_electrodes_for);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->return_electrodes_form= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_return_electrodes_for.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_return_electrodes_for */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_return_electrodes_val(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
Finds the id of the return_ electrodes value in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_return_electrodes_val);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->return_electrodes_value= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_return_electrodes_val.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_return_electrodes_val */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ch_pacing_return_electrodes_val(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
Called when the return electrodes widget is changed.
==============================================================================*/
{
	char *new_value,*return_electrodes;
	struct Pacing_window *pacing_window;
	XmAnyCallbackStruct *text_data

	ENTER(ch_pacing_return_electrodes_val);
	USE_PARAMETER(widget_id);
	if ((text_data=(XmAnyCallbackStruct *)call_data)&&
		((XmCR_ACTIVATE==text_data->reason)||
		(XmCR_LOSING_FOCUS==text_data->reason)))
	{
		if (pacing_window=(struct Pacing_window *)pacing_window_structure)
		{
			if (!(pacing_window->pacing))
			{
				XtVaGetValues(pacing_window->return_electrodes_value,
					XmNvalue,&new_value,
					NULL);
				set_return_channels_from_string(pacing_window,new_value);
			}
			if (!(return_electrodes=pacing_window->return_electrodes))
			{
				return_electrodes="";
			}
			XtVaSetValues(pacing_window->return_electrodes_value,
				XmNcursorPosition,strlen(return_electrodes),
				XmNvalue,return_electrodes,
				NULL);
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"ch_pacing_return_electrodes_val.  Missing pacing_window_structure");
		}
	}
	LEAVE;
} /* ch_pacing_return_electrodes_val */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_control_voltage_form(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Finds the id of the control voltage form in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_control_voltage_form);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->control_voltage_form= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_control_voltage_form.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_control_voltage_form */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_control_voltage_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Finds the id of the control voltage value in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_control_voltage_value);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->control_voltage_value= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_control_voltage_value.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_control_voltage_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ch_pacing_control_voltage_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Called when the control voltage widget is changed.
==============================================================================*/
{
	char *new_value,value_string[20];
	float control_voltage;
	struct Pacing_window *pacing_window;
	XmAnyCallbackStruct *text_data

	ENTER(ch_pacing_control_voltage_value);
	USE_PARAMETER(widget_id);
	if ((text_data=(XmAnyCallbackStruct *)call_data)&&
		((XmCR_ACTIVATE==text_data->reason)||
		(XmCR_LOSING_FOCUS==text_data->reason)))
	{
		if (pacing_window=(struct Pacing_window *)pacing_window_structure)
		{
			if (!(pacing_window->pacing))
			{
				XtVaGetValues(pacing_window->control_voltage_value,
					XmNvalue,&new_value,
					NULL);
				if ((1==sscanf(new_value,"%f",&control_voltage))&&(0<control_voltage))
				{
					pacing_window->control_voltage=control_voltage;
				}
			}
			sprintf(value_string,"%g",pacing_window->control_voltage);
			XtVaSetValues(pacing_window->control_voltage_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"ch_pacing_control_voltage_value.  Missing pacing_window_structure");
		}
	}
	LEAVE;
} /* ch_pacing_control_voltage_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_basic_cycle_length_fo(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Finds the id of the basic cycle length form in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_basic_cycle_length_fo);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->basic_cycle_length_form= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_basic_cycle_length_fo.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_basic_cycle_length_fo */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_basic_cycle_length_va(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Finds the id of the basic cycle length value in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_basic_cycle_length_va);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->basic_cycle_length_value= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_basic_cycle_length_va.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_basic_cycle_length_va */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ch_pacing_basic_cycle_length_va(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Called when the basic cycle length widget is changed.
==============================================================================*/
{
	char *new_value,value_string[20];
	float basic_cycle_length;
	struct Pacing_window *pacing_window;
	XmAnyCallbackStruct *text_data

	ENTER(ch_pacing_basic_cycle_length_va);
	USE_PARAMETER(widget_id);
	if ((text_data=(XmAnyCallbackStruct *)call_data)&&
		((XmCR_ACTIVATE==text_data->reason)||
		(XmCR_LOSING_FOCUS==text_data->reason)))
	{
		if (pacing_window=(struct Pacing_window *)pacing_window_structure)
		{
			if (!(pacing_window->pacing))
			{
				XtVaGetValues(pacing_window->basic_cycle_length_value,
					XmNvalue,&new_value,
					NULL);
				if (1==sscanf(new_value,"%f",&basic_cycle_length))
				{
					/* must be a multiple of the S2 resolution */
					if (basic_cycle_length<pacing_window->s2_resolution)
					{
						basic_cycle_length=pacing_window->s2_resolution;
					}
					else
					{
						basic_cycle_length=(pacing_window->s2_resolution)*
							floor(basic_cycle_length/(pacing_window->s2_resolution)+0.5);
					}
					pacing_window->basic_cycle_length=basic_cycle_length;
				}
			}
			sprintf(value_string,"%g",pacing_window->basic_cycle_length);
			XtVaSetValues(pacing_window->basic_cycle_length_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"ch_pacing_basic_cycle_length_va.  Missing pacing_window_structure");
		}
	}
	LEAVE;
} /* ch_pacing_basic_cycle_length_va */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_basic_cycle_length_pa(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 12 October 2001

DESCRIPTION :
Finds the id of the basic cycle length pace button in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_basic_cycle_length_pa);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->basic_cycle_length_pace_toggle= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_basic_cycle_length_pa.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_basic_cycle_length_pa */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ch_pacing_basic_cycle_length_pa(Widget widget,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
Called when the basic cycle length pace button is toggled.
==============================================================================*/
{
	Boolean status;
	float *pacing_voltages,voltages_per_second;
	int i,number_of_pacing_voltages;
	struct Pacing_window *pacing_window;

	ENTER(ch_pacing_basic_cycle_length_pa);
	USE_PARAMETER(call_data);
	unemap_stop_stimulating(0);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		XtVaGetValues(widget,XmNset,&status,NULL);
		if (True==status)
		{
			number_of_pacing_voltages=2*(int)((pacing_window->basic_cycle_length)/
				(pacing_window->s2_resolution)+0.5);
			if (REALLOCATE(pacing_voltages,pacing_window->pacing_voltages,float,
				number_of_pacing_voltages))
			{
				pacing_window->pacing_voltages=pacing_voltages;
				/* set control sensitivity */
				XtSetSensitive(pacing_window->restitution_time_pace_toggle,False);
				XtSetSensitive(pacing_window->restitution_curve_pace_toggle,False);
				/* set up pacing */
				pacing_voltages[0]=pacing_window->control_voltage;
				for (i=0;i<number_of_pacing_voltages;i++)
				{
					pacing_voltages[i]=0;
				}
				pacing_voltages[number_of_pacing_voltages/2]=
					pacing_window->control_voltage;
				voltages_per_second=2000./(pacing_window->s2_resolution);
				for (i=0;i<pacing_window->number_of_pacing_channels;i++)
				{
					unemap_set_channel_stimulating((pacing_window->pacing_channels)[i],1);
				}
				unemap_load_voltage_stimulating(
					pacing_window->number_of_pacing_channels,
					pacing_window->pacing_channels,number_of_pacing_voltages,
					voltages_per_second,pacing_voltages,(unsigned int)0);
				for (i=0;i<pacing_window->number_of_return_channels;i++)
				{
					unemap_set_channel_stimulating((pacing_window->return_channels)[i],1);
				}
				unemap_load_voltage_stimulating(
					pacing_window->number_of_return_channels,
					pacing_window->return_channels,(int)0,(float)0,(float *)NULL,
					(unsigned int)0);
				/* start pacing */
				unemap_start_stimulating();
				pacing_window->pacing=1;
			}
			else
			{
				status=False;
				XtVaSetValues(widget,XmNset,status,NULL);
			}
		}
		else
		{
			/* stop pacing */
			unemap_stop_stimulating(0);
			for (i=0;i<pacing_window->number_of_pacing_channels;i++)
			{
				unemap_set_channel_stimulating((pacing_window->pacing_channels)[i],0);
			}
			for (i=0;i<pacing_window->number_of_return_channels;i++)
			{
				unemap_set_channel_stimulating((pacing_window->return_channels)[i],0);
			}
			pacing_window->pacing=0;
			/* set control sensitivity */
			XtSetSensitive(pacing_window->restitution_time_pace_toggle,True);
			XtSetSensitive(pacing_window->restitution_curve_pace_toggle,True);
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"ch_pacing_basic_cycle_length_pa.  Missing pacing_window_structure");
	}
	LEAVE;
} /* ch_pacing_basic_cycle_length_pa */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_number_of_s1_form(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Finds the id of the number of S1 beats form in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_number_of_s1_form);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->number_of_s1_form= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_number_of_s1_form.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_number_of_s1_form */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_number_of_s1_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Finds the id of the number of S1 beats value in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_number_of_s1_value);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->number_of_s1_value= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_number_of_s1_value.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_number_of_s1_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ch_pacing_number_of_s1_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Called when the number of s1 widget is changed.
==============================================================================*/
{
	char *new_value,value_string[20];
	int number_of_s1;
	struct Pacing_window *pacing_window;
	XmAnyCallbackStruct *text_data

	ENTER(ch_pacing_number_of_s1_value);
	USE_PARAMETER(widget_id);
	if ((text_data=(XmAnyCallbackStruct *)call_data)&&
		((XmCR_ACTIVATE==text_data->reason)||
		(XmCR_LOSING_FOCUS==text_data->reason)))
	{
		if (pacing_window=(struct Pacing_window *)pacing_window_structure)
		{
			if (!(pacing_window->pacing))
			{
				XtVaGetValues(pacing_window->number_of_s1_value,
					XmNvalue,&new_value,
					NULL);
				if ((1==sscanf(new_value,"%d",&number_of_s1))&&(0<number_of_s1))
				{
					pacing_window->number_of_s1=number_of_s1;
				}
			}
			sprintf(value_string,"%d",pacing_window->number_of_s1);
			XtVaSetValues(pacing_window->number_of_s1_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"ch_pacing_number_of_s1_value.  Missing pacing_window_structure");
		}
	}
	LEAVE;
} /* ch_pacing_number_of_s1_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_s2_resolution_form(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Finds the id of the S2 resolution form in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_s2_resolution_form);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->s2_resolution_form= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_s2_resolution_form.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_s2_resolution_form */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_s2_resolution_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Finds the id of the S2 resolution value in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_s2_resolution_value);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->s2_resolution_value= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_s2_resolution_value.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_s2_resolution_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ch_pacing_s2_resolution_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Called when the s2 resolution widget is changed.
==============================================================================*/
{
	char *new_value,value_string[20];
	float s2_resolution;
	struct Pacing_window *pacing_window;
	XmAnyCallbackStruct *text_data

	ENTER(ch_pacing_s2_resolution_value);
	USE_PARAMETER(widget_id);
	if ((text_data=(XmAnyCallbackStruct *)call_data)&&
		((XmCR_ACTIVATE==text_data->reason)||
		(XmCR_LOSING_FOCUS==text_data->reason)))
	{
		if (pacing_window=(struct Pacing_window *)pacing_window_structure)
		{
			if (!(pacing_window->pacing))
			{
				XtVaGetValues(pacing_window->s2_resolution_value,
					XmNvalue,&new_value,
					NULL);
				if ((1==sscanf(new_value,"%f",&s2_resolution))&&(0<s2_resolution))
				{
					pacing_window->s2_resolution=s2_resolution;
				}
			}
			sprintf(value_string,"%g",pacing_window->s2_resolution);
			XtVaSetValues(pacing_window->s2_resolution_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"ch_pacing_s2_resolution_value.  Missing pacing_window_structure");
		}
	}
	LEAVE;
} /* ch_pacing_s2_resolution_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_s2_delay_form(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Finds the id of the S2 delay form in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_s2_delay_form);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->s2_delay_form= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_s2_delay_form.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_s2_delay_form */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_s2_delay_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Finds the id of the S2 delay value in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_s2_delay_value);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->s2_delay_value= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_s2_delay_value.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_s2_delay_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ch_pacing_s2_delay_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 18 October 2001

DESCRIPTION :
Called when the s2 delay widget is changed.
==============================================================================*/
{
	char *new_value,value_string[20];
	float s2_delay;
	struct Pacing_window *pacing_window;
	XmAnyCallbackStruct *text_data

	ENTER(ch_pacing_s2_delay_value);
	USE_PARAMETER(widget_id);
	if ((text_data=(XmAnyCallbackStruct *)call_data)&&
		((XmCR_ACTIVATE==text_data->reason)||
		(XmCR_LOSING_FOCUS==text_data->reason)))
	{
		if (pacing_window=(struct Pacing_window *)pacing_window_structure)
		{
			if (!(pacing_window->pacing))
			{
				XtVaGetValues(pacing_window->s2_delay_value,
					XmNvalue,&new_value,
					NULL);
				if (1==sscanf(new_value,"%f",&s2_delay))
				{
					/* must be a multiple of the S2 resolution */
					if (s2_delay<pacing_window->s2_resolution)
					{
						s2_delay=pacing_window->s2_resolution;
					}
					else
					{
						s2_delay=(pacing_window->s2_resolution)*
							floor(s2_delay/(pacing_window->s2_resolution)+0.5);
					}
					pacing_window->s2_delay=s2_delay;
				}
			}
			sprintf(value_string,"%g",pacing_window->s2_delay);
			XtVaSetValues(pacing_window->s2_delay_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"ch_pacing_s2_delay_value.  Missing pacing_window_structure");
		}
	}
	LEAVE;
} /* ch_pacing_s2_delay_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_s2_delay_change_form(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Finds the id of the S2 delay change form in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_s2_delay_change_form);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->s2_delay_change_form= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_s2_delay_change_form.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_s2_delay_change_form */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_s2_delay_change_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Finds the id of the S2 delay change value in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_s2_delay_change_value);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->s2_delay_change_value= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_s2_delay_change_value.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_s2_delay_change_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ch_pacing_s2_delay_change_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 18 October 2001

DESCRIPTION :
Called when the s2 delay change widget is changed.
==============================================================================*/
{
	char *new_value,value_string[20];
	float s2_delay_change;
	struct Pacing_window *pacing_window;
	XmAnyCallbackStruct *text_data

	ENTER(ch_pacing_s2_delay_change_value);
	USE_PARAMETER(widget_id);
	if ((text_data=(XmAnyCallbackStruct *)call_data)&&
		((XmCR_ACTIVATE==text_data->reason)||
		(XmCR_LOSING_FOCUS==text_data->reason)))
	{
		if (pacing_window=(struct Pacing_window *)pacing_window_structure)
		{
			if (!(pacing_window->pacing))
			{
				XtVaGetValues(pacing_window->s2_delay_change_value,
					XmNvalue,&new_value,
					NULL);
				if (1==sscanf(new_value,"%f",&s2_delay_change))
				{
					/* must be a multiple of the S2 resolution */
					if (s2_delay_change<pacing_window->s2_resolution)
					{
						s2_delay_change=pacing_window->s2_resolution;
					}
					else
					{
						s2_delay_change=(pacing_window->s2_resolution)*
							floor(s2_delay_change/(pacing_window->s2_resolution)+0.5);
					}
					pacing_window->s2_delay_change=s2_delay_change;
				}
			}
			sprintf(value_string,"%g",pacing_window->s2_delay_change);
			XtVaSetValues(pacing_window->s2_delay_change_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"ch_pacing_s2_delay_change_value.  Missing pacing_window_structure");
		}
	}
	LEAVE;
} /* ch_pacing_s2_delay_change_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_s2_delay_factor_form(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Finds the id of the S2 delay change factor form in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_s2_delay_factor_form);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->s2_delay_change_factor_form= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_s2_delay_factor_form.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_s2_delay_factor_form */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_s2_delay_factor_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Finds the id of the S2 delay change factor value in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_s2_delay_factor_value);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->s2_delay_change_factor_value= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_s2_delay_factor_value.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_s2_delay_factor_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ch_pacing_s2_delay_factor_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Called when the s2 delay change factor widget is changed.
==============================================================================*/
{
	char *new_value,value_string[20];
	float s2_delay_change_factor;
	struct Pacing_window *pacing_window;
	XmAnyCallbackStruct *text_data

	ENTER(ch_pacing_s2_delay_factor_value);
	USE_PARAMETER(widget_id);
	if ((text_data=(XmAnyCallbackStruct *)call_data)&&
		((XmCR_ACTIVATE==text_data->reason)||
		(XmCR_LOSING_FOCUS==text_data->reason)))
	{
		if (pacing_window=(struct Pacing_window *)pacing_window_structure)
		{
			if (!(pacing_window->pacing))
			{
				XtVaGetValues(pacing_window->s2_delay_change_factor_value,
					XmNvalue,&new_value,
					NULL);
				if ((1==sscanf(new_value,"%f",&s2_delay_change_factor))&&
					(1<s2_delay_change_factor))
				{
					pacing_window->s2_delay_change_factor=s2_delay_change_factor;
				}
			}
			sprintf(value_string,"%g",pacing_window->s2_delay_change_factor);
			XtVaSetValues(pacing_window->s2_delay_change_factor_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"ch_pacing_s2_delay_factor_value.  Missing pacing_window_structure");
		}
	}
	LEAVE;
} /* ch_pacing_s2_delay_factor_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_restitution_time_pace(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 12 October 2001

DESCRIPTION :
Finds the id of the restitution time pace button in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_restitution_time_pace);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->restitution_time_pace_toggle= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_restitution_time_pace.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_restitution_time_pace */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ch_pacing_restitution_time_pace(Widget widget,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
Called when the restitution time pace button is toggled.
==============================================================================*/
{
	Boolean status;
	float *pacing_voltages,voltages_per_second;
	int i,j,number_of_pacing_voltages,number_of_s1_voltages,
		number_of_s2_voltages;
	struct Pacing_window *pacing_window;

	ENTER(ch_pacing_restitution_time_pace);
	USE_PARAMETER(call_data);
	unemap_stop_stimulating(0);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		XtVaGetValues(widget,XmNset,&status,NULL);
		if (True==status)
		{
			number_of_pacing_voltages=2*((pacing_window->number_of_s1)+3)*
				((int)((pacing_window->basic_cycle_length)/
				(pacing_window->s2_resolution))+1);
			if (REALLOCATE(pacing_voltages,pacing_window->pacing_voltages,float,
				number_of_pacing_voltages))
			{
				pacing_window->pacing_voltages=pacing_voltages;
				/* set control sensitivity */
				XtSetSensitive(pacing_window->restitution_time_yes_button,True);
				XtSetSensitive(pacing_window->restitution_time_no_button,True);
				XtSetSensitive(pacing_window->basic_cycle_length_pace_toggle,False);
				XtSetSensitive(pacing_window->restitution_curve_pace_toggle,False);
				/* set up pacing */
				number_of_s1_voltages=
					2*(int)((pacing_window->basic_cycle_length)/
					(pacing_window->s2_resolution)+0.5);
				number_of_pacing_voltages=0;
				for (j=0;j<number_of_s1_voltages;j++)
				{
					pacing_voltages[number_of_pacing_voltages]=0;
					number_of_pacing_voltages++;
				}
				for (i=0;i<(pacing_window->number_of_s1)-1;i++)
				{
					pacing_voltages[number_of_pacing_voltages]=
						pacing_window->control_voltage;
					number_of_pacing_voltages++;
					for (j=number_of_s1_voltages-1;j>0;j--)
					{
						pacing_voltages[number_of_pacing_voltages]=0;
						number_of_pacing_voltages++;
					}
				}
				pacing_window->total_number_of_s1_pacing_voltages=
					number_of_pacing_voltages;
				pacing_window->response='f';
				number_of_s2_voltages=2*(int)((pacing_window->s2_delay)/
					(pacing_window->s2_resolution)+0.5);
				pacing_voltages[number_of_pacing_voltages]=
					pacing_window->control_voltage;
				number_of_pacing_voltages++;
				for (j=number_of_s2_voltages-1;j>0;j--)
				{
					pacing_voltages[number_of_pacing_voltages]=0;
					number_of_pacing_voltages++;
				}
				pacing_voltages[number_of_pacing_voltages]=
					pacing_window->control_voltage;
				number_of_pacing_voltages++;
				pacing_voltages[number_of_pacing_voltages]=0;
				number_of_pacing_voltages++;
				voltages_per_second=2000./(pacing_window->s2_resolution);
				for (i=0;i<pacing_window->number_of_pacing_channels;i++)
				{
					unemap_set_channel_stimulating((pacing_window->pacing_channels)[i],1);
				}
				unemap_load_voltage_stimulating(
					pacing_window->number_of_pacing_channels,
					pacing_window->pacing_channels,number_of_pacing_voltages,
					voltages_per_second,pacing_voltages,(unsigned int)1);
				for (i=0;i<pacing_window->number_of_return_channels;i++)
				{
					unemap_set_channel_stimulating((pacing_window->return_channels)[i],1);
				}
				unemap_load_voltage_stimulating(
					pacing_window->number_of_return_channels,
					pacing_window->return_channels,(int)0,(float)0,(float *)NULL,
					(unsigned int)0);
				/* start pacing */
				unemap_start_stimulating();
				pacing_window->pacing=1;
				/* print information to standard out */
				printf("Restitution Pacing on with\n");
				printf("BCL = %g ms\n",pacing_window->basic_cycle_length);
				printf("Number of S1 stimulations = %d\n",pacing_window->number_of_s1);
				printf("Initial S2 delay = %g ms\n",pacing_window->s2_delay);
				printf("Initial S2 delay change = %g ms\n",
					pacing_window->s2_delay_change);
				printf("Delay factor = %g\n",pacing_window->s2_delay_change_factor);
				printf("S2 resolution = %g ms\n",pacing_window->s2_resolution);
			}
			else
			{
				status=False;
				XtVaSetValues(widget,XmNset,status,NULL);
			}
		}
		else
		{
			/* stop pacing */
			unemap_stop_stimulating(0);
			for (i=0;i<pacing_window->number_of_pacing_channels;i++)
			{
				unemap_set_channel_stimulating((pacing_window->pacing_channels)[i],0);
			}
			for (i=0;i<pacing_window->number_of_return_channels;i++)
			{
				unemap_set_channel_stimulating((pacing_window->return_channels)[i],0);
			}
			/* set control sensitivity */
			XtSetSensitive(pacing_window->restitution_time_yes_button,False);
			XtSetSensitive(pacing_window->restitution_time_no_button,False);
			XtSetSensitive(pacing_window->basic_cycle_length_pace_toggle,True);
			XtSetSensitive(pacing_window->restitution_curve_pace_toggle,True);
			if (pacing_window->pacing)
			{
				/* print information to standard out */
				printf("Restitution Pacing off\n");
			}
			pacing_window->pacing=0;
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"ch_pacing_restitution_time_pace.  Missing pacing_window_structure");
	}
	LEAVE;
} /* ch_pacing_restitution_time_pace */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_restitution_time_yes(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 12 October 2001

DESCRIPTION :
Finds the id of the restitution time yes button in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_restitution_time_yes);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->restitution_time_yes_button= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_restitution_time_yes.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_restitution_time_yes */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ac_pacing_restitution_time_yes(Widget widget,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
Called when the restitution time yes button is pressed.
==============================================================================*/
{
	char value_string[21];
	float *pacing_voltages,s2_delay,voltages_per_second;
	int i,number_of_pacing_voltages,number_of_s2_voltages,stop;
	struct Pacing_window *pacing_window;

	ENTER(ac_pacing_restitution_time_yes);
	USE_PARAMETER(call_data);
	USE_PARAMETER(widget);
	unemap_stop_stimulating(0);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		/* update pacing */
		stop=0;
		if (('f'!=pacing_window->response)&&('y'!=pacing_window->response))
		{
			if (pacing_window->s2_delay_change>pacing_window->s2_resolution)
			{
				pacing_window->s2_delay_change /= pacing_window->s2_delay_change_factor;
				/* must be a multiple of the S2 resolution */
				if (pacing_window->s2_delay_change<pacing_window->s2_resolution)
				{
					pacing_window->s2_delay_change=pacing_window->s2_resolution;
				}
				else
				{
					pacing_window->s2_delay_change=(pacing_window->s2_resolution)*
						floor((pacing_window->s2_delay_change)/
						(pacing_window->s2_resolution)+0.5);
				}
			}
			else
			{
				pacing_window->s2_delay_change=pacing_window->s2_resolution;
				stop=1;
			}
			sprintf(value_string,"%g",pacing_window->s2_delay_change);
			XtVaSetValues(pacing_window->s2_delay_change_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
		}
		/* print information to standard out */
		printf("Captured OK: current S2 delay change = %g ms, current S2 delay = %g ms\n",
			pacing_window->s2_delay_change,pacing_window->s2_delay);
		s2_delay=(pacing_window->s2_delay)-(pacing_window->s2_delay_change);
		pacing_window->response='y';
		if (!stop&&(pacing_window->s2_resolution<=s2_delay)&&
			(s2_delay<2*(pacing_window->basic_cycle_length)))
		{
			pacing_window->s2_delay=s2_delay;
			sprintf(value_string,"%g",s2_delay);
			XtVaSetValues(pacing_window->s2_delay_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
			pacing_voltages=pacing_window->pacing_voltages;
			number_of_pacing_voltages=
				pacing_window->total_number_of_s1_pacing_voltages;
			number_of_s2_voltages=2*(int)((pacing_window->s2_delay)/
				(pacing_window->s2_resolution)+0.5);
			pacing_voltages[number_of_pacing_voltages]=
				pacing_window->control_voltage;
			number_of_pacing_voltages++;
			for (i=number_of_s2_voltages-1;i>0;i--)
			{
				pacing_voltages[number_of_pacing_voltages]=0;
				number_of_pacing_voltages++;
			}
			pacing_voltages[number_of_pacing_voltages]=
				pacing_window->control_voltage;
			number_of_pacing_voltages++;
			pacing_voltages[number_of_pacing_voltages]=0;
			number_of_pacing_voltages++;
			voltages_per_second=2000./(pacing_window->s2_resolution);
			unemap_load_voltage_stimulating(
				pacing_window->number_of_pacing_channels,
				pacing_window->pacing_channels,number_of_pacing_voltages,
				voltages_per_second,pacing_voltages,(unsigned int)1);
			unemap_load_voltage_stimulating(
				pacing_window->number_of_return_channels,
				pacing_window->return_channels,(int)0,(float)0,(float *)NULL,
				(unsigned int)0);
			/* start pacing */
			unemap_start_stimulating();
		}
		else
		{
			/* have found restitution time */
			XtVaSetValues(pacing_window->restitution_time_pace_toggle,
				XmNset,False,
				NULL);
			XtSetSensitive(pacing_window->restitution_time_yes_button,False);
			XtSetSensitive(pacing_window->restitution_time_no_button,False);
			XtSetSensitive(pacing_window->basic_cycle_length_pace_toggle,True);
			XtSetSensitive(pacing_window->restitution_curve_pace_toggle,True);
			for (i=0;i<pacing_window->number_of_pacing_channels;i++)
			{
				unemap_set_channel_stimulating((pacing_window->pacing_channels)[i],0);
			}
			for (i=0;i<pacing_window->number_of_return_channels;i++)
			{
				unemap_set_channel_stimulating((pacing_window->return_channels)[i],0);
			}
			/* print information to standard out */
			printf("Restitution Pacing off\n");
			pacing_window->pacing=0;
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"ac_pacing_restitution_time_yes.  Missing pacing_window_structure");
	}
	LEAVE;
} /* ac_pacing_restitution_time_yes */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_restitution_time_no(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 12 October 2001

DESCRIPTION :
Finds the id of the restitution time no button in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_restitution_time_no);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->restitution_time_no_button= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_restitution_time_no.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_restitution_time_no */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ac_pacing_restitution_time_no(Widget widget,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
Called when the restitution time no button is pressed.
==============================================================================*/
{
	char value_string[21];
	float *pacing_voltages,s2_delay,voltages_per_second;
	int i,number_of_pacing_voltages,number_of_s2_voltages,stop;
	struct Pacing_window *pacing_window;

	ENTER(ac_pacing_restitution_time_no);
	USE_PARAMETER(call_data);
	USE_PARAMETER(widget);
	unemap_stop_stimulating(0);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		/* update pacing */
		stop=0;
		if (('f'!=pacing_window->response)&&('n'!=pacing_window->response))
		{
			if (pacing_window->s2_delay_change>pacing_window->s2_resolution)
			{
				pacing_window->s2_delay_change /= pacing_window->s2_delay_change_factor;
				/* must be a multiple of the S2 resolution */
				if (pacing_window->s2_delay_change<pacing_window->s2_resolution)
				{
					pacing_window->s2_delay_change=pacing_window->s2_resolution;
				}
				else
				{
					pacing_window->s2_delay_change=(pacing_window->s2_resolution)*
						floor((pacing_window->s2_delay_change)/
						(pacing_window->s2_resolution)+0.5);
				}
			}
			else
			{
				pacing_window->s2_delay_change=pacing_window->s2_resolution;
				stop=1;
			}
			sprintf(value_string,"%g",pacing_window->s2_delay_change);
			XtVaSetValues(pacing_window->s2_delay_change_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
		}
		/* print information to standard out */
		printf("No capture: current S2 delay change = %g ms, current S2 delay = %g ms\n",
			pacing_window->s2_delay_change,pacing_window->s2_delay);
		s2_delay=(pacing_window->s2_delay)+(pacing_window->s2_delay_change);
		pacing_window->response='n';
		if (!stop&&(pacing_window->s2_resolution<=s2_delay)&&
			(s2_delay<2*(pacing_window->basic_cycle_length)))
		{
			pacing_window->s2_delay=s2_delay;
			sprintf(value_string,"%g",s2_delay);
			XtVaSetValues(pacing_window->s2_delay_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
			pacing_voltages=pacing_window->pacing_voltages;
			number_of_pacing_voltages=
				pacing_window->total_number_of_s1_pacing_voltages;
			number_of_s2_voltages=2*(int)((pacing_window->s2_delay)/
				(pacing_window->s2_resolution)+0.5);
			pacing_voltages[number_of_pacing_voltages]=
				pacing_window->control_voltage;
			number_of_pacing_voltages++;
			for (i=number_of_s2_voltages-1;i>0;i--)
			{
				pacing_voltages[number_of_pacing_voltages]=0;
				number_of_pacing_voltages++;
			}
			pacing_voltages[number_of_pacing_voltages]=
				pacing_window->control_voltage;
			number_of_pacing_voltages++;
			pacing_voltages[number_of_pacing_voltages]=0;
			number_of_pacing_voltages++;
			voltages_per_second=2000./(pacing_window->s2_resolution);
			unemap_load_voltage_stimulating(
				pacing_window->number_of_pacing_channels,
				pacing_window->pacing_channels,number_of_pacing_voltages,
				voltages_per_second,pacing_voltages,(unsigned int)1);
			unemap_load_voltage_stimulating(
				pacing_window->number_of_return_channels,
				pacing_window->return_channels,(int)0,(float)0,(float *)NULL,
				(unsigned int)0);
			/* start pacing */
			unemap_start_stimulating();
		}
		else
		{
			/* have found restitution time */
			XtVaSetValues(pacing_window->restitution_time_pace_toggle,
				XmNset,False,
				NULL);
			XtSetSensitive(pacing_window->restitution_time_yes_button,False);
			XtSetSensitive(pacing_window->restitution_time_no_button,False);
			XtSetSensitive(pacing_window->basic_cycle_length_pace_toggle,True);
			XtSetSensitive(pacing_window->restitution_curve_pace_toggle,True);
			for (i=0;i<pacing_window->number_of_pacing_channels;i++)
			{
				unemap_set_channel_stimulating((pacing_window->pacing_channels)[i],0);
			}
			for (i=0;i<pacing_window->number_of_return_channels;i++)
			{
				unemap_set_channel_stimulating((pacing_window->return_channels)[i],0);
			}
			/* print information to standard out */
			printf("Restitution Pacing off\n");
			pacing_window->pacing=0;
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"ac_pacing_restitution_time_no.  Missing pacing_window_structure");
	}
	LEAVE;
} /* ac_pacing_restitution_time_no */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_s2_s1_pause_form(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 4 November 2001

DESCRIPTION :
Finds the id of the S2-S1 pause form in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_s2_s1_pause_form);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->s2_s1_pause_form= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_s2_s1_pause_form.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_s2_s1_pause_form */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_s2_s1_pause_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 4 November 2001

DESCRIPTION :
Finds the id of the S2-S1 pause value in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_s2_s1_pause_value);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->s2_s1_pause_value= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_s2_s1_pause_value.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_s2_s1_pause_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ch_pacing_s2_s1_pause_value(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 4 November 2001

DESCRIPTION :
Called when the S2-S1 pause widget is changed.
==============================================================================*/
{
	char *new_value,value_string[20];
	float s2_s1_pause;
	struct Pacing_window *pacing_window;
	XmAnyCallbackStruct *text_data

	ENTER(ch_pacing_s2_s1_pause_value);
	USE_PARAMETER(widget_id);
	if ((text_data=(XmAnyCallbackStruct *)call_data)&&
		((XmCR_ACTIVATE==text_data->reason)||
		(XmCR_LOSING_FOCUS==text_data->reason)))
	{
		if (pacing_window=(struct Pacing_window *)pacing_window_structure)
		{
			if (!(pacing_window->pacing))
			{
				XtVaGetValues(pacing_window->s2_s1_pause_value,
					XmNvalue,&new_value,
					NULL);
				if (1==sscanf(new_value,"%f",&s2_s1_pause))
				{
					/* must be a multiple of the S2 resolution */
					if (s2_s1_pause<pacing_window->s2_resolution)
					{
						s2_s1_pause=pacing_window->s2_resolution;
					}
					else
					{
						s2_s1_pause=(pacing_window->s2_resolution)*
							floor(s2_s1_pause/(pacing_window->s2_resolution)+0.5);
					}
					pacing_window->s2_s1_pause=s2_s1_pause;
				}
			}
			sprintf(value_string,"%g",pacing_window->s2_s1_pause);
			XtVaSetValues(pacing_window->s2_s1_pause_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"ch_pacing_s2_s1_pause_value.  Missing pacing_window_structure");
		}
	}
	LEAVE;
} /* ch_pacing_s2_s1_pause_value */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_decrement_threshold_f(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 4 November 2001

DESCRIPTION :
Finds the id of the pacing decrement threshold pairs form in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_decrement_threshold_f);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->decrement_threshold_pairs_form= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_decrement_threshold_f.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_decrement_threshold_f */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_decrement_threshold_v(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 4 November 2001

DESCRIPTION :
Finds the id of the pacing decrement threshold pairs value in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_decrement_threshold_value);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->decrement_threshold_pairs_value= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_decrement_threshold_v.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_decrement_threshold_v */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ch_pacing_decrement_threshold_v(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 4 November 2001

DESCRIPTION :
Called when the pacing decrement threshold pairs widget is changed.
==============================================================================*/
{
	char *new_value,*decrement_threshold_pairs_string;
	struct Pacing_window *pacing_window;
	XmAnyCallbackStruct *text_data

	ENTER(ch_pacing_decrement_threshold_v);
	USE_PARAMETER(widget_id);
	if ((text_data=(XmAnyCallbackStruct *)call_data)&&
		((XmCR_ACTIVATE==text_data->reason)||
		(XmCR_LOSING_FOCUS==text_data->reason)))
	{
		if (pacing_window=(struct Pacing_window *)pacing_window_structure)
		{
			if (!(pacing_window->pacing))
			{
				XtVaGetValues(pacing_window->decrement_threshold_pairs_value,
					XmNvalue,&new_value,
					NULL);
				set_decrement_threshold_pairs_from_string(pacing_window,new_value);
			}
			if (!(decrement_threshold_pairs_string=
				pacing_window->decrement_threshold_pairs_string))
			{
				decrement_threshold_pairs_string="";
			}
			XtVaSetValues(pacing_window->decrement_threshold_pairs_value,
				XmNcursorPosition,strlen(decrement_threshold_pairs_string),
				XmNvalue,decrement_threshold_pairs_string,
				NULL);
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"ch_pacing_decrement_threshold_v.  Missing pacing_window_structure");
		}
	}
	LEAVE;
} /* ch_pacing_decrement_threshold_v */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void id_pacing_restitution_curve_pac(Widget *widget_id,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 4 November 2001

DESCRIPTION :
Finds the id of the restitution curve pace button in the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(id_pacing_restitution_curve_pac);
	USE_PARAMETER(call_data);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		pacing_window->restitution_curve_pace_toggle= *widget_id;
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"id_pacing_restitution_curve_pac.  Missing pacing_window_structure");
	}
	LEAVE;
} /* id_pacing_restitution_curve_pac */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void ch_pacing_restitution_curve_pac(Widget widget,
	XtPointer pacing_window_structure,XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
Called when the restitution curve pace button is toggled.
==============================================================================*/
{
	Boolean status;
	float *pacing_voltages,s2_delay,voltages_per_second;
	int i,j,k,last,number_of_pacing_voltages,number_of_s1_pacing_voltages,
		number_of_s2_pacing_voltages,number_of_s2_s1_pacing_voltages,return_code,
		s2_delay_count;
	struct Pacing_window *pacing_window;

	ENTER(ch_pacing_restitution_curve_pac);
	USE_PARAMETER(call_data);
	unemap_stop_stimulating(0);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		XtVaGetValues(widget,XmNset,&status,NULL);
		if (True==status)
		{
			return_code=0;
			/* print information to standard out */
			printf("Restitution Curve Pacing start with\n");
			printf("BCL = %g ms\n",pacing_window->basic_cycle_length);
			printf("Number of S1 stimulations = %d\n",pacing_window->number_of_s1);
			printf("S2 resolution = %g ms\n",pacing_window->s2_resolution);
			printf("S2-S1 pause = %g ms\n",pacing_window->s2_s1_pause);
			for (i=0;i<pacing_window->number_of_decrement_threshold_pairs;i++)
			{
				printf("%d.  S2 delay decrement = %g ms.  S2 delay threshold = %g ms\n",
					i+1,(pacing_window->decrement_threshold_pairs)[2*i],
					(pacing_window->decrement_threshold_pairs)[2*i+1]);
			}
			number_of_s2_s1_pacing_voltages=2*(int)((pacing_window->s2_s1_pause)/
				(pacing_window->s2_resolution)+0.5);
			if (REALLOCATE(pacing_voltages,pacing_window->pacing_voltages,float,
				number_of_s2_s1_pacing_voltages))
			{
				pacing_window->pacing_voltages=pacing_voltages;
				for (i=number_of_s2_s1_pacing_voltages;i>0;i--)
				{
					*pacing_voltages=0;
					pacing_voltages++;
				}
				number_of_pacing_voltages=number_of_s2_s1_pacing_voltages;
				number_of_s1_pacing_voltages=
					2*(int)((pacing_window->basic_cycle_length)/
					(pacing_window->s2_resolution)+0.5);
				s2_delay=pacing_window->s2_delay;
				return_code=1;
				i=0;
				last=0;
				s2_delay_count=0;
				while (return_code&&(last||
					(i<pacing_window->number_of_decrement_threshold_pairs)))
				{
					while (return_code&&
						(last||((i<pacing_window->number_of_decrement_threshold_pairs)&&
						(s2_delay>(pacing_window->decrement_threshold_pairs)[2*i+1]))))
					{
						number_of_s2_pacing_voltages=2*(int)(s2_delay/
							(pacing_window->s2_resolution)+0.5);
						if (REALLOCATE(pacing_voltages,pacing_window->pacing_voltages,float,
							number_of_pacing_voltages+number_of_s2_s1_pacing_voltages+
							(pacing_window->number_of_s1-1)*number_of_s1_pacing_voltages+
							number_of_s2_pacing_voltages))
						{
							/* print information to standard out */
							s2_delay_count++;
							printf("%d) S2 delay = %g ms\n",s2_delay_count,s2_delay);
							pacing_window->pacing_voltages=pacing_voltages;
							pacing_voltages += number_of_pacing_voltages;
							number_of_pacing_voltages += number_of_s2_s1_pacing_voltages+
								(pacing_window->number_of_s1-1)*number_of_s1_pacing_voltages+
								number_of_s2_pacing_voltages;
							for (j=pacing_window->number_of_s1-1;j--;j>0)
							{
								*pacing_voltages=pacing_window->control_voltage;
								pacing_voltages++;
								for (k=number_of_s1_pacing_voltages-1;k>0;k--)
								{
									*pacing_voltages=0;
									pacing_voltages++;
								}
							}
							*pacing_voltages=pacing_window->control_voltage;
							pacing_voltages++;
							for (j=number_of_s2_pacing_voltages-1;j>0;j--)
							{
								*pacing_voltages=0;
								pacing_voltages++;
							}
							*pacing_voltages=pacing_window->control_voltage;
							pacing_voltages++;
							for (j=number_of_s2_s1_pacing_voltages-1;j>0;j--)
							{
								*pacing_voltages=0;
								pacing_voltages++;
							}
							s2_delay -= (pacing_window->decrement_threshold_pairs)[2*i];
							/* must be a multiple of the S2 resolution */
							if (s2_delay<pacing_window->s2_resolution)
							{
								s2_delay=pacing_window->s2_resolution;
							}
							else
							{
								s2_delay=(pacing_window->s2_resolution)*
									floor(s2_delay/(pacing_window->s2_resolution)+0.5);
							}
						}
						else
						{
							return_code=0;
						}
						last=0;
					}
					if (i<pacing_window->number_of_decrement_threshold_pairs)
					{
						s2_delay=(pacing_window->decrement_threshold_pairs)[2*i+1];
						if (i+1==pacing_window->number_of_decrement_threshold_pairs)
						{
							last=1;
						}
					}
					i++;
				}
			}
			/* print information to standard out */
			printf("Restitution Curve Pacing finish\n");
			if (return_code&&(0<number_of_pacing_voltages))
			{
				/* set control sensitivity */
				XtSetSensitive(pacing_window->restitution_time_pace_toggle,False);
				XtSetSensitive(pacing_window->basic_cycle_length_pace_toggle,False);
				/* set up pacing */
				voltages_per_second=2000./(pacing_window->s2_resolution);
				for (i=0;i<pacing_window->number_of_pacing_channels;i++)
				{
					unemap_set_channel_stimulating((pacing_window->pacing_channels)[i],1);
				}
				unemap_load_voltage_stimulating(
					pacing_window->number_of_pacing_channels,
					pacing_window->pacing_channels,number_of_pacing_voltages,
					voltages_per_second,pacing_window->pacing_voltages,(unsigned int)1);
				for (i=0;i<pacing_window->number_of_return_channels;i++)
				{
					unemap_set_channel_stimulating((pacing_window->return_channels)[i],1);
				}
				unemap_load_voltage_stimulating(
					pacing_window->number_of_return_channels,
					pacing_window->return_channels,(int)0,(float)0,(float *)NULL,
					(unsigned int)0);
				/* start pacing */
				unemap_start_stimulating();
				pacing_window->pacing=1;
			}
			else
			{
				status=False;
				XtVaSetValues(widget,XmNset,status,NULL);
			}
		}
		else
		{
			/* stop pacing */
			unemap_stop_stimulating(0);
			for (i=0;i<pacing_window->number_of_pacing_channels;i++)
			{
				unemap_set_channel_stimulating((pacing_window->pacing_channels)[i],0);
			}
			for (i=0;i<pacing_window->number_of_return_channels;i++)
			{
				unemap_set_channel_stimulating((pacing_window->return_channels)[i],0);
			}
			pacing_window->pacing=0;
			/* set control sensitivity */
			XtSetSensitive(pacing_window->restitution_time_pace_toggle,True);
			XtSetSensitive(pacing_window->basic_cycle_length_pace_toggle,True);
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"ch_pacing_restitution_curve_pac.  Missing pacing_window_structure");
	}
	LEAVE;
} /* ch_pacing_restitution_curve_pac */
#endif /* defined (MOTIF) */

#if defined (MOTIF)
static void close_Pacing_window(Widget widget,XtPointer pacing_window_structure,
	XtPointer call_data)
/*******************************************************************************
LAST MODIFIED : 14 October 2001

DESCRIPTION :
Closes the windows associated with the pacing window.
==============================================================================*/
{
	struct Pacing_window *pacing_window;

	ENTER(close_Pacing_window);
	USE_PARAMETER(widget);
	USE_PARAMETER(call_data);
	unemap_stop_stimulating(0);
	if (pacing_window=(struct Pacing_window *)pacing_window_structure)
	{
		/* close the pacing window box */
		XtPopdown(pacing_window->shell);
		/* unghost the activation button */
		XtSetSensitive(pacing_window->activation,True);
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"close_Pacing_window.  Missing pacing_window_structure");
	}
	LEAVE;
} /* close_Pacing_window */
#endif /* defined (MOTIF) */

static struct Pacing_window *create_Pacing_window(
	struct Pacing_window **pacing_window_address,struct Rig **rig_address,
#if defined (MOTIF)
	Widget activation,
#endif /* defined (MOTIF) */
	struct User_interface *user_interface)
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
Allocates the memory for a pacing window.  Retrieves the necessary widgets and
initializes the appropriate fields.
==============================================================================*/
{
#if defined (MOTIF)
	char *decrement_threshold_pairs_string,*pacing_electrodes,*return_electrodes;
	MrmType pacing_window_class;
	static MrmRegisterArg callback_list[]={
		{"close_Pacing_window",(XtPointer)close_Pacing_window},
		{"destroy_Pacing_window_callback",
		(XtPointer)destroy_Pacing_window_callback},
		{"id_pacing_electrodes_form",(XtPointer)id_pacing_electrodes_form},
		{"id_pacing_electrodes_value",(XtPointer)id_pacing_electrodes_value},
		{"ch_pacing_electrodes_value",(XtPointer)ch_pacing_electrodes_value},
		{"id_pacing_return_electrodes_for",
		(XtPointer)id_pacing_return_electrodes_for},
		{"id_pacing_return_electrodes_val",
		(XtPointer)id_pacing_return_electrodes_val},
		{"ch_pacing_return_electrodes_val",
		(XtPointer)ch_pacing_return_electrodes_val},
		{"id_pacing_control_voltage_form",
		(XtPointer)id_pacing_control_voltage_form},
		{"id_pacing_control_voltage_value",
		(XtPointer)id_pacing_control_voltage_value},
		{"ch_pacing_control_voltage_value",
		(XtPointer)ch_pacing_control_voltage_value},
		{"id_pacing_basic_cycle_length_fo",
		(XtPointer)id_pacing_basic_cycle_length_fo},
		{"id_pacing_basic_cycle_length_va",
		(XtPointer)id_pacing_basic_cycle_length_va},
		{"ch_pacing_basic_cycle_length_va",
		(XtPointer)ch_pacing_basic_cycle_length_va},
		{"id_pacing_basic_cycle_length_pa",
		(XtPointer)id_pacing_basic_cycle_length_pa},
		{"ch_pacing_basic_cycle_length_pa",
		(XtPointer)ch_pacing_basic_cycle_length_pa},
		{"id_pacing_number_of_s1_form",(XtPointer)id_pacing_number_of_s1_form},
		{"id_pacing_number_of_s1_value",(XtPointer)id_pacing_number_of_s1_value},
		{"ch_pacing_number_of_s1_value",(XtPointer)ch_pacing_number_of_s1_value},
		{"id_pacing_s2_resolution_form",(XtPointer)id_pacing_s2_resolution_form},
		{"id_pacing_s2_resolution_value",(XtPointer)id_pacing_s2_resolution_value},
		{"ch_pacing_s2_resolution_value",(XtPointer)ch_pacing_s2_resolution_value},
		{"id_pacing_s2_delay_form",(XtPointer)id_pacing_s2_delay_form},
		{"id_pacing_s2_delay_value",(XtPointer)id_pacing_s2_delay_value},
		{"ch_pacing_s2_delay_value",(XtPointer)ch_pacing_s2_delay_value},
		{"id_pacing_s2_delay_change_form",
		(XtPointer)id_pacing_s2_delay_change_form},
		{"id_pacing_s2_delay_change_value",
		(XtPointer)id_pacing_s2_delay_change_value},
		{"ch_pacing_s2_delay_change_value",
		(XtPointer)ch_pacing_s2_delay_change_value},
		{"id_pacing_s2_delay_factor_form",
		(XtPointer)id_pacing_s2_delay_factor_form},
		{"id_pacing_s2_delay_factor_value",
		(XtPointer)id_pacing_s2_delay_factor_value},
		{"ch_pacing_s2_delay_factor_value",
		(XtPointer)ch_pacing_s2_delay_factor_value},
		{"id_pacing_restitution_time_pace",
		(XtPointer)id_pacing_restitution_time_pace},
		{"ch_pacing_restitution_time_pace",
		(XtPointer)ch_pacing_restitution_time_pace},
		{"id_pacing_restitution_time_yes",
		(XtPointer)id_pacing_restitution_time_yes},
		{"ac_pacing_restitution_time_yes",
		(XtPointer)ac_pacing_restitution_time_yes},
		{"id_pacing_restitution_time_no",(XtPointer)id_pacing_restitution_time_no},
		{"ac_pacing_restitution_time_no",(XtPointer)ac_pacing_restitution_time_no},
		{"id_pacing_s2_s1_pause_form",(XtPointer)id_pacing_s2_s1_pause_form},
		{"id_pacing_s2_s1_pause_value",(XtPointer)id_pacing_s2_s1_pause_value},
		{"ch_pacing_s2_s1_pause_value",(XtPointer)ch_pacing_s2_s1_pause_value},
		{"id_pacing_decrement_threshold_f",
		(XtPointer)id_pacing_decrement_threshold_f},
		{"id_pacing_decrement_threshold_v",
		(XtPointer)id_pacing_decrement_threshold_v},
		{"ch_pacing_decrement_threshold_v",
		(XtPointer)ch_pacing_decrement_threshold_v},
		{"id_pacing_restitution_curve_pac",
		(XtPointer)id_pacing_restitution_curve_pac},
		{"ch_pacing_restitution_curve_pac",
		(XtPointer)ch_pacing_restitution_curve_pac}
		};
	static MrmRegisterArg identifier_list[]=
	{
		{"pacing_window_structure",(XtPointer)NULL}
	};
#define XmNrestitutionBasicCycleLength "restitutionBasicCycleLength"
#define XmCRestitutionBasicCycleLength "RestitutionBasicCycleLength"
#define XmNrestitutionControlVoltage "restitutionControlVoltage"
#define XmCRestitutionControlVoltage "RestitutionControlVoltage"
#define XmNrestitutionDecrementThresholdPairs "restitutionDecrementThresholdPairs"
#define XmCRestitutionDecrementThresholdPairs "RestitutionDecrementThresholdPairs"
#define XmNrestitutionNumberOfS1 "restitutionNumberOfS1"
#define XmCRestitutionNumberOfS1 "RestitutionNumberOfS1"
#define XmNrestitutionPacingElectrodes "restitutionPacingElectrodes"
#define XmCRestitutionPacingElectrodes "RestitutionPacingElectrodes"
#define XmNrestitutionReturnElectrodes "restitutionReturnElectrodes"
#define XmCRestitutionReturnElectrodes "RestitutionReturnElectrodes"
#define XmNrestitutionS2Delay "restitutionS2Delay"
#define XmCRestitutionS2Delay "RestitutionS2Delay"
#define XmNrestitutionS2DelayChange "restitutionS2DelayChange"
#define XmCRestitutionS2DelayChange "RestitutionS2DelayChange"
#define XmNrestitutionS2DelayChangeFactor "restitutionS2DelayChangeFactor"
#define XmCRestitutionS2DelayChangeFactor "RestitutionS2DelayChangeFactor"
#define XmNrestitutionS2Resolution "restitutionS2Resolution"
#define XmCRestitutionS2Resolution "RestitutionS2Resolution"
#define XmNrestitutionS2S1Pause "restitutionS2S1Pause"
#define XmCRestitutionS2S1Pause "RestitutionS2S1Pause"
	static XtResource resources[]=
	{
		{
			XmNrestitutionBasicCycleLength,
			XmCRestitutionBasicCycleLength,
			XmRFloat,
			sizeof(float),
			XtOffsetOf(Pacing_window_settings,basic_cycle_length),
			XmRString,
			"600"
		},
		{
			XmNrestitutionControlVoltage,
			XmCRestitutionControlVoltage,
			XmRFloat,
			sizeof(float),
			XtOffsetOf(Pacing_window_settings,control_voltage),
			XmRString,
			"1"
		},
		{
			XmNrestitutionNumberOfS1,
			XmCRestitutionNumberOfS1,
			XmRInt,
			sizeof(int),
			XtOffsetOf(Pacing_window_settings,number_of_s1),
			XmRString,
			"9"
		},
		{
			XmNrestitutionS2Delay,
			XmCRestitutionS2Delay,
			XmRFloat,
			sizeof(float),
			XtOffsetOf(Pacing_window_settings,s2_delay),
			XmRString,
			"500"
		},
		{
			XmNrestitutionS2DelayChange,
			XmCRestitutionS2DelayChange,
			XmRFloat,
			sizeof(float),
			XtOffsetOf(Pacing_window_settings,s2_delay_change),
			XmRString,
			"100"
		},
		{
			XmNrestitutionS2DelayChangeFactor,
			XmCRestitutionS2DelayChangeFactor,
			XmRFloat,
			sizeof(float),
			XtOffsetOf(Pacing_window_settings,s2_delay_change_factor),
			XmRString,
			"2"
		},
		{
			XmNrestitutionS2Resolution,
			XmCRestitutionS2Resolution,
			XmRFloat,
			sizeof(float),
			XtOffsetOf(Pacing_window_settings,s2_resolution),
			XmRString,
			"20"
		},
		{
			XmNrestitutionS2S1Pause,
			XmCRestitutionS2S1Pause,
			XmRFloat,
			sizeof(float),
			XtOffsetOf(Pacing_window_settings,s2_s1_pause),
			XmRString,
			"800"
		},
	};
	static XtResource pacing_electrodes_resources[]=
	{
		{
			XmNrestitutionPacingElectrodes,
			XmCRestitutionPacingElectrodes,
			XmRString,
			sizeof(String),
			0,
			XmRString,
			""
		},
	};
	static XtResource return_electrodes_resources[]=
	{
		{
			XmNrestitutionReturnElectrodes,
			XmCRestitutionReturnElectrodes,
			XmRString,
			sizeof(String),
			0,
			XmRString,
			""
		},
	};
	static XtResource decrement_threshold_pairs_resources[]=
	{
		{
			XmNrestitutionDecrementThresholdPairs,
			XmCRestitutionDecrementThresholdPairs,
			XmRString,
			sizeof(String),
			0,
			XmRString,
			""
		},
	};
#endif /* defined (MOTIF) */
	struct Pacing_window *pacing_window;

	ENTER(create_Pacing_window);
	pacing_window=(struct Pacing_window *)NULL;
	/* check arguments */
	if (rig_address&&user_interface)
	{
#if defined (MOTIF)
		if (MrmOpenHierarchy_base64_string(pacing_window_uidh,
			&pacing_window_hierarchy,&pacing_window_hierarchy_open))
		{
#endif /* defined (MOTIF) */
			if (ALLOCATE(pacing_window,struct Pacing_window,1))
			{
				pacing_window->address=pacing_window_address;
				pacing_window->rig_address=rig_address;
				pacing_window->user_interface=user_interface;
				pacing_window->pacing=0;
				pacing_window->pacing_voltages=(float *)NULL;
				pacing_window->number_of_pacing_channels=0;
				pacing_window->pacing_channels=(int *)NULL;
				pacing_window->pacing_electrodes=(char *)NULL;
				pacing_window->number_of_return_channels=0;
				pacing_window->return_channels=(int *)NULL;
				pacing_window->return_electrodes=(char *)NULL;
				pacing_window->number_of_decrement_threshold_pairs=0;
				pacing_window->decrement_threshold_pairs=(float *)NULL;
				pacing_window->decrement_threshold_pairs_string=(char *)NULL;
#if defined (MOTIF)
				pacing_window->activation=activation;
				pacing_window->window=(Widget)NULL;
				pacing_window->shell=(Widget)NULL;
				pacing_window->basic_cycle_length_pace_toggle=(Widget)NULL;
				pacing_window->restitution_time_pace_toggle=(Widget)NULL;
				pacing_window->restitution_time_no_button=(Widget)NULL;
				pacing_window->restitution_time_yes_button=(Widget)NULL;
				pacing_window->basic_cycle_length_form=(Widget)NULL;
				pacing_window->basic_cycle_length_value=(Widget)NULL;
				pacing_window->control_voltage_form=(Widget)NULL;
				pacing_window->control_voltage_value=(Widget)NULL;
				pacing_window->number_of_s1_form=(Widget)NULL;
				pacing_window->number_of_s1_value=(Widget)NULL;
				pacing_window->pacing_electrodes_form=(Widget)NULL;
				pacing_window->pacing_electrodes_value=(Widget)NULL;
				pacing_window->return_electrodes_form=(Widget)NULL;
				pacing_window->return_electrodes_value=(Widget)NULL;
				pacing_window->s2_delay_form=(Widget)NULL;
				pacing_window->s2_delay_value=(Widget)NULL;
				pacing_window->s2_delay_change_form=(Widget)NULL;
				pacing_window->s2_delay_change_value=(Widget)NULL;
				pacing_window->s2_delay_change_factor_form=(Widget)NULL;
				pacing_window->s2_delay_change_factor_value=(Widget)NULL;
				pacing_window->s2_resolution_form=(Widget)NULL;
				pacing_window->s2_resolution_value=(Widget)NULL;
				pacing_window->s2_s1_pause_form=(Widget)NULL;
				pacing_window->s2_s1_pause_value=(Widget)NULL;
				pacing_window->decrement_threshold_pairs_form=(Widget)NULL;
				pacing_window->decrement_threshold_pairs_value=(Widget)NULL;
				pacing_window->restitution_curve_pace_toggle=(Widget)NULL;
				/* retrieve the settings */
				XtVaGetApplicationResources(user_interface->application_shell,
					pacing_window,resources,XtNumber(resources),NULL);
				pacing_electrodes=(char *)NULL;
				XtVaGetApplicationResources(user_interface->application_shell,
					&pacing_electrodes,pacing_electrodes_resources,
					XtNumber(pacing_electrodes_resources),NULL);
				/* NB.  XtVaGetApplicationResources does not allocate memory for
					pacing_electrodes, so it does not need to be free'd */
				set_pacing_channels_from_string(pacing_window,pacing_electrodes);
				return_electrodes=(char *)NULL;
				XtVaGetApplicationResources(user_interface->application_shell,
					&return_electrodes,return_electrodes_resources,
					XtNumber(return_electrodes_resources),NULL);
				/* NB.  XtVaGetApplicationResources does not allocate memory for
					return_electrodes, so it does not need to be free'd */
				set_return_channels_from_string(pacing_window,return_electrodes);
				decrement_threshold_pairs_string=(char *)NULL;
				XtVaGetApplicationResources(user_interface->application_shell,
					&decrement_threshold_pairs_string,decrement_threshold_pairs_resources,
					XtNumber(decrement_threshold_pairs_resources),NULL);
				/* NB.  XtVaGetApplicationResources does not allocate memory for
					decrement_threshold_pairs, so it does not need to be free'd */
				set_decrement_threshold_pairs_from_string(pacing_window,
					decrement_threshold_pairs_string);
#endif /* defined (MOTIF) */
#if defined (MOTIF)
				/* create the window shell */
				if (pacing_window->shell=XtVaCreatePopupShell(
					"pacing_window_shell",
					topLevelShellWidgetClass,user_interface->application_shell,
					XmNallowShellResize,False,
					XmNuserData,user_interface,
					NULL))
				{
					/*???DB.  What is this for? */
					XtAddCallback(pacing_window->shell,XmNdestroyCallback,
						destroy_window_shell,(XtPointer)create_Shell_list_item(
						&(pacing_window->shell),user_interface));
					/* register the other callbacks */
					if (MrmSUCCESS==MrmRegisterNamesInHierarchy(pacing_window_hierarchy,
						callback_list,XtNumber(callback_list)))
					{
						/* assign and register the identifiers */
						identifier_list[0].value=(XtPointer)pacing_window;
						if (MrmSUCCESS==MrmRegisterNamesInHierarchy(pacing_window_hierarchy,
							identifier_list,XtNumber(identifier_list)))
						{
							/* fetch the window widget */
							if (MrmSUCCESS==MrmFetchWidget(pacing_window_hierarchy,
								"pacing_window",pacing_window->shell,&(pacing_window->window),
								&pacing_window_class))
							{
								XtManageChild(pacing_window->window);
#endif /* defined (MOTIF) */
								if (pacing_window_address)
								{
									*pacing_window_address=pacing_window;
								}
								/*???DB.  Anything else? */
#if defined (MOTIF)
							}
							else
							{
								display_message(ERROR_MESSAGE,
									"create_Pacing_window.  Could not fetch the window widget");
								XtDestroyWidget(pacing_window->shell);
								DEALLOCATE(pacing_window);
							}
						}
						else
						{
							display_message(ERROR_MESSAGE,
								"create_Pacing_window.  Could not register identifiers");
							XtDestroyWidget(pacing_window->shell);
							DEALLOCATE(pacing_window);
						}
					}
					else
					{
						display_message(ERROR_MESSAGE,
							"create_Pacing_window.  Could not register callbacks");
						XtDestroyWidget(pacing_window->shell);
						DEALLOCATE(pacing_window);
					}
				}
				else
				{
					display_message(ERROR_MESSAGE,
						"create_Pacing_window.  Could not create window shell");
					DEALLOCATE(pacing_window);
				}
#endif /* defined (MOTIF) */
			}
			else
			{
				display_message(ERROR_MESSAGE,
					"create_Pacing_window.  Insufficient memory for pacing window");
			}
#if defined (MOTIF)
		}
		else
		{
			display_message(ERROR_MESSAGE,
				"create_Pacing_window.  Could not open hierarchy");
		}
#endif /* defined (MOTIF) */
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"create_Pacing_window.  Invalid argument(s).  %p %p",rig_address,
			user_interface);
	}
	LEAVE;

	return (pacing_window);
} /* create_Pacing_window */

/*
Global functions
----------------
*/
struct Pacing_window *open_Pacing_window(
	struct Pacing_window **pacing_window_address,struct Rig **rig_address,
#if defined (MOTIF)
	Widget activation,
#endif /* defined (MOTIF) */
	struct User_interface *user_interface)
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
If the pacing window does not exist, it is created.  The pacing window is
opened.
==============================================================================*/
{
	struct Pacing_window *pacing_window;
#if defined (MOTIF)
	char *decrement_threshold_pairs_string,*pacing_electrodes,
		*return_electrodes,value_string[20];
#endif /* defined (MOTIF) */

	ENTER(open_pacing_window);
	pacing_window=(struct Pacing_window *)NULL;
	if (pacing_window_address)
	{
		if (!(pacing_window= *pacing_window_address))
		{
			pacing_window=create_Pacing_window(pacing_window_address,rig_address,
				activation,user_interface);
		}
		if (pacing_window)
		{
			/* make sure that the window is consistent with its values */
#if defined (MOTIF)
			if (!(pacing_electrodes=pacing_window->pacing_electrodes))
			{
				pacing_electrodes="";
			}
			XtVaSetValues(pacing_window->pacing_electrodes_value,
				XmNcursorPosition,strlen(pacing_electrodes),
				XmNvalue,pacing_electrodes,
				NULL);
#endif /* defined (MOTIF) */
#if defined (MOTIF)
			if (!(return_electrodes=pacing_window->return_electrodes))
			{
				return_electrodes="";
			}
			XtVaSetValues(pacing_window->return_electrodes_value,
				XmNcursorPosition,strlen(return_electrodes),
				XmNvalue,return_electrodes,
				NULL);
#endif /* defined (MOTIF) */
			if (pacing_window->basic_cycle_length<=0)
			{
				pacing_window->basic_cycle_length=600;
			}
#if defined (MOTIF)
			sprintf(value_string,"%g",pacing_window->basic_cycle_length);
			XtVaSetValues(pacing_window->basic_cycle_length_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
#endif /* defined (MOTIF) */
			if (pacing_window->control_voltage<=0)
			{
				pacing_window->control_voltage=1;
			}
#if defined (MOTIF)
			sprintf(value_string,"%g",pacing_window->control_voltage);
			XtVaSetValues(pacing_window->control_voltage_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
#endif /* defined (MOTIF) */
			if (pacing_window->number_of_s1<1)
			{
				pacing_window->number_of_s1=9;
			}
#if defined (MOTIF)
			sprintf(value_string,"%d",pacing_window->number_of_s1);
			XtVaSetValues(pacing_window->number_of_s1_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
#endif /* defined (MOTIF) */
			if (pacing_window->s2_resolution<=0)
			{
				pacing_window->s2_resolution=20;
			}
#if defined (MOTIF)
			sprintf(value_string,"%g",pacing_window->s2_resolution);
			XtVaSetValues(pacing_window->s2_resolution_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
#endif /* defined (MOTIF) */
			if (pacing_window->s2_delay<=0)
			{
				pacing_window->s2_delay=500;
			}
			/* must be a multiple of the S2 resolution */
			if (pacing_window->s2_delay<pacing_window->s2_resolution)
			{
				pacing_window->s2_delay=pacing_window->s2_resolution;
			}
			else
			{
				pacing_window->s2_delay=(pacing_window->s2_resolution)*
					floor((pacing_window->s2_delay)/(pacing_window->s2_resolution)+0.5);
			}
#if defined (MOTIF)
			sprintf(value_string,"%g",pacing_window->s2_delay);
			XtVaSetValues(pacing_window->s2_delay_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
#endif /* defined (MOTIF) */
			if (pacing_window->s2_delay_change<=0)
			{
				pacing_window->s2_delay_change=100;
			}
			/* must be a multiple of the S2 resolution */
			if (pacing_window->s2_delay_change<pacing_window->s2_resolution)
			{
				pacing_window->s2_delay_change=pacing_window->s2_resolution;
			}
			else
			{
				pacing_window->s2_delay_change=(pacing_window->s2_resolution)*
					floor((pacing_window->s2_delay_change)/
					(pacing_window->s2_resolution)+0.5);
			}
#if defined (MOTIF)
			sprintf(value_string,"%g",pacing_window->s2_delay_change);
			XtVaSetValues(pacing_window->s2_delay_change_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
#endif /* defined (MOTIF) */
			if (pacing_window->s2_delay_change_factor<=1)
			{
				pacing_window->s2_delay_change_factor=2;
			}
#if defined (MOTIF)
			sprintf(value_string,"%g",pacing_window->s2_delay_change_factor);
			XtVaSetValues(pacing_window->s2_delay_change_factor_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
#endif /* defined (MOTIF) */
#if defined (MOTIF)
			XtSetSensitive(pacing_window->restitution_time_yes_button,False);
			XtSetSensitive(pacing_window->restitution_time_no_button,False);
#endif /* defined (MOTIF) */
			if (pacing_window->s2_s1_pause<=0)
			{
				pacing_window->s2_s1_pause=800;
			}
			/* must be a multiple of the S2 resolution */
			if (pacing_window->s2_s1_pause<pacing_window->s2_resolution)
			{
				pacing_window->s2_s1_pause=pacing_window->s2_resolution;
			}
			else
			{
				pacing_window->s2_s1_pause=(pacing_window->s2_resolution)*
					floor((pacing_window->s2_s1_pause)/(pacing_window->s2_resolution)+
					0.5);
			}
#if defined (MOTIF)
			sprintf(value_string,"%g",pacing_window->s2_s1_pause);
			XtVaSetValues(pacing_window->s2_s1_pause_value,
				XmNcursorPosition,strlen(value_string),
				XmNvalue,value_string,
				NULL);
#endif /* defined (MOTIF) */
#if defined (MOTIF)
			if (!(decrement_threshold_pairs_string=
				pacing_window->decrement_threshold_pairs_string))
			{
				decrement_threshold_pairs_string="";
			}
			XtVaSetValues(pacing_window->decrement_threshold_pairs_value,
				XmNcursorPosition,strlen(decrement_threshold_pairs_string),
				XmNvalue,decrement_threshold_pairs_string,
				NULL);
#endif /* defined (MOTIF) */
#if defined (MOTIF)
			if (pacing_window->activation)
			{
				/* ghost the activation button */
				XtSetSensitive(pacing_window->activation,False);
				/* pop up the pacing window */
				XtPopup(pacing_window->shell,XtGrabNone);
			}
#endif /* defined (MOTIF) */
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"open_pacing_window.  Missing pacing_window_address");
	}
	LEAVE;

	return (pacing_window);
} /* open_pacing_window */

int destroy_Pacing_window(struct Pacing_window **pacing_window_address)
/*******************************************************************************
LAST MODIFIED : 11 November 2001

DESCRIPTION :
If the <address> field of the pacing window is not NULL, <*address> is set to
NULL.  If the <activation> field is not NULL, the <activation> widget is
unghosted.  The function frees the memory associated with the fields of the
pacing window and frees the memory associated with the pacing window.
==============================================================================*/
{
	int return_code;
	struct Pacing_window *pacing_window;

	ENTER(destroy_Pacing_window);
	return_code=0;
	unemap_stop_stimulating(0);
	if (pacing_window_address)
	{
		if (pacing_window= *pacing_window_address)
		{
			return_code=1;
			if (pacing_window->shell)
			{
				XtPopdown(pacing_window->shell);
			}
			if (pacing_window->address)
			{
				*(pacing_window->address)=(struct Pacing_window *)NULL;
			}
#if defined (MOTIF)
			/* unghost the activation button */
			XtSetSensitive(pacing_window->activation,True);
#endif /* defined (MOTIF) */
			DEALLOCATE(pacing_window->pacing_voltages);
			DEALLOCATE(pacing_window->pacing_channels);
			DEALLOCATE(pacing_window->pacing_electrodes);
			DEALLOCATE(pacing_window->return_channels);
			DEALLOCATE(pacing_window->return_electrodes);
			DEALLOCATE(pacing_window->decrement_threshold_pairs_string);
			DEALLOCATE(pacing_window->decrement_threshold_pairs);
			DEALLOCATE(pacing_window);
			*pacing_window_address=(struct Pacing_window *)NULL;
		}
	}
	else
	{
		display_message(ERROR_MESSAGE,
			"destroy_Pacing_window.  Missing pacing_window_address");
	}
	LEAVE;
} /* destroy_Pacing_window */
