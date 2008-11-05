<?
/*
   This file is part of Asternic call center stats.

    Asternic call center stats is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Asternic call center stats is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Asternic call center stats.  If not, see <http://www.gnu.org/licenses/>.
*/

$dayp[0] = "Sunday";
$dayp[1] = "Monday";
$dayp[2] = "Tuesday";
$dayp[3] = "Wednesday";
$dayp[4] = "Thursday";
$dayp[5] = "Friday";
$dayp[6] = "Saturday";

$yearp[0] = "January";
$yearp[1] = "February";
$yearp[2] = "March";
$yearp[3] = "April";
$yearp[4] = "May";
$yearp[5] = "June";
$yearp[6] = "July";
$yearp[7] = "August";
$yearp[8] = "September";
$yearp[9] = "October";
$yearp[10]= "November";
$yearp[11]= "December";

// Menu options
$lang['en']['menu_home']         = "Home";
$lang['en']['menu_answered']     = "Answered";
$lang['en']['menu_unanswered']   = "Unanswered";
$lang['en']['menu_distribution'] = "Distribution";
$lang['en']['menu_realtime'] = "Realtime";

// tooltips
$lang['en']['pdfhelp'] = "Exports the data to a .pdf file";
$lang['en']['csvhelp'] = "Exports the data to a comma separated file, to be read by your spreadsheet software";
$lang['en']['gotop']   = "Goes to the top of the page";

// Index page
$lang['en']['ALL']               = "ALL";
$lang['en']['lower']             = "Lower  ...";
$lang['en']['higher']            = "Higher ...";
$lang['en']['select_queue']      = "Select Queues";
$lang['en']['select_agent']      = "Select Agents";
$lang['en']['select_timeframe']  = "Select Time Frame";
$lang['en']['queue']   	         = "Queue";
$lang['en']['start']   	         = "Start Date";
$lang['en']['end']   	         = "End Date";
$lang['en']['display_report']    = "Display Report";
$lang['en']['shortcuts']         = "Shortcuts";
$lang['en']['today']             = "Today";
$lang['en']['this_week']         = "This week";
$lang['en']['this_month']        = "This month";
$lang['en']['last_three_months'] = "Last 3 months";
$lang['en']['available']         = "Available";
$lang['en']['selected']          = "Selected";
$lang['en']['invaliddate']       = "Invalid date range";

// Answered page
$lang['en']['answered_calls_by_agent'] = "Answered Calls by Agent";
$lang['en']['answered_calls_by_queue'] = "Answered Calls by Queue";
$lang['en']['anws_unanws_by_hour']     = "Answered/Unanswered by Hour";
$lang['en']['report_info']       = "Report Info";
$lang['en']['period']            = "Period";
$lang['en']['answered_calls']    = "Answered Calls";
$lang['en']['transferred_calls'] = "Transferred Calls";
$lang['en']['secs']              = "secs";
$lang['en']['minutes']           = "min";
$lang['en']['hours']             = "hs";
$lang['en']['calls']             = "calls";
$lang['en']['Calls']             = "Calls";
$lang['en']['agent']             = "Agent";
$lang['en']['avg']               = "Avg";
$lang['en']['avg_calltime']      = "Avg Durat.";
$lang['en']['avg_holdtime']      = "Avg Hold";
$lang['en']['percent']           = "%";
$lang['en']['total']             = "Total";
$lang['en']['calltime']          = "Call Time";
$lang['en']['holdtime']          = "Hold Time";
$lang['en']['total_time_agent']  = "Total Time per Agent (secs)";
$lang['en']['no_calls_agent']    = "Number of Calls per Agent";
$lang['en']['call_response']     = "Service Level";
$lang['en']['within']            = "Within ";
$lang['en']['answer']            = "Answer";
$lang['en']['count']             = "Count";
$lang['en']['delta']             = "Delta";
$lang['en']['disconnect_cause']  = "Disconnection Cause";
$lang['en']['cause']             = "Cause";
$lang['en']['agent_hungup']      = "Agent hung up";
$lang['en']['caller_hungup']     = "Caller hung up";
$lang['en']['caller']            = "Caller";
$lang['en']['transfers']         = "Transfers";
$lang['en']['to']                = "To";

// Unanswered page
$lang['en']['unanswered_calls']    = "Unanswered Calls";
$lang['en']['number_unanswered']   = "Number of Unanswered Calls";
$lang['en']['avg_wait_before_dis'] = "Avg wait time before disconnect";
$lang['en']['avg_queue_pos_at_dis']= "Avg queue position at disconnection";
$lang['en']['avg_queue_start']     = "Avg start queue position";
$lang['en']['user_abandon']        = "User Abandon";
$lang['en']['abandon']             = "Abandon";
$lang['en']['timeout']             = "Timeout";
$lang['en']['unanswered_calls_qu'] = "Unanswered Calls by Queue";

// Distribution
$lang['en']['totals']              = "Totals";
$lang['en']['number_answered']     = "Number of Answered Calls";
$lang['en']['number_unanswered']   = "Number of Unanswered Calls";
$lang['en']['agent_login']         = "Agent Login";
$lang['en']['agent_logoff']        = "Agent Logoff";
$lang['en']['call_distrib_day']    = "Call Distribution per day";
$lang['en']['call_distrib_hour']   = "Call Distribution per hour";
$lang['en']['call_distrib_week']   = "Call Distribution per day of week";
$lang['en']['date']                = "Date";
$lang['en']['day']                 = "Day";
$lang['en']['days']                = "days";
$lang['en']['hour']                = "Hour";
$lang['en']['answered']            = "Answered";
$lang['en']['unanswered']          = "Unanswered";
$lang['en']['percent_answered']    = "% Answ";
$lang['en']['percent_unanswered']  = "% Unansw";
$lang['en']['login']               = "Login";
$lang['en']['logoff']              = "Logoff";
$lang['en']['answ_by_day']         = "Answered Calls by day of week";
$lang['en']['unansw_by_day']       = "Unanswered Calls by day of week";
$lang['en']['avg_call_time_by_day']= "Average Call Time by day of week";
$lang['en']['avg_hold_time_by_day']= "Average Hold Time by day of week";
$lang['en']['answ_by_hour']        = "Answered Calls by hour";
$lang['en']['unansw_by_hour']      = "Unanswered Calls by hour";
$lang['en']['avg_call_time_by_hr'] = "Average Call Time by hour";
$lang['en']['avg_hold_time_by_hr'] = "Average Hold Time by hour";
$lang['en']['page']                = "Page";
$lang['en']['export']              = "Export table:";

// Realtime
$lang['en']['realtime_agent_status']                    = "Agents Status";
$lang['en']['realtime_agent_hide']                      = "Hide";
$lang['en']['realtime_callwaiting_detail']              = "Call waiting detail";
$lang['en']['realtime_callwaiting_queue']               = "Queue";
$lang['en']['realtime_callwaiting_position']            = "Position";
$lang['en']['realtime_callwaiting_callerid']            = "Callerid";
$lang['en']['realtime_callwaiting_waitime']             = "Wait time";
$lang['en']['realtime_agentstatus']                     = "Agents";
$lang['en']['realtime_agentstatus_queue']               = "Queue";
$lang['en']['realtime_agentstatus_agent']               = "Agent";
$lang['en']['realtime_agentstatus_state']               = "State";
$lang['en']['realtime_agentstatus_duration']            = "Dur.";
$lang['en']['realtime_agentstatus_clid']                = "CLID";
$lang['en']['realtime_agentstatus_lastincall']          = "Last in call";
$lang['en']['realtime_agent_summary']                   = "Queue summary";
$lang['en']['realtime_agent_summary_queue']             = "Queue";
$lang['en']['realtime_agent_summary_staffed']           = "Staffed";
$lang['en']['realtime_agent_summary_talking']           = "Talking";
$lang['en']['realtime_agent_summary_paused']            = "Paused";
$lang['en']['realtime_agent_summary_callswaiting']      = "Calls Waiting";
$lang['en']['realtime_agent_summary_oldcallwaiting']    = "Old Call Waiting";


?>
