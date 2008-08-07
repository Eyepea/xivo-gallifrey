/*
 * Asterisk -- A telephony toolkit for Linux.
 *
 * Trivial application to send a TIFF file as a FAX
 * 
 * Copyright (C) 2003, Steve Underwood
 *
 * Steve Underwood <steveu@coppice.org>
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License
 */
 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <pthread.h>
#include <errno.h>
#include <tiffio.h>

#include <spandsp.h>

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision:$")

#include "asterisk/lock.h"
#include "asterisk/file.h"
#include "asterisk/logger.h"
#include "asterisk/channel.h"
#include "asterisk/pbx.h"
#include "asterisk/module.h"

static char *tdesc = "Trivial FAX Transmit Application";

static char *app = "TxFAX";

static char *synopsis = "Send a FAX file";

static char *descrip = 
"  TxFAX(filename[|caller][|debug]):  Send a given TIFF file to the channel as a FAX.\n"
"The \"caller\" option makes the application behave as a calling machine,\n"
"rather than the answering machine. The default behaviour is to behave as\n"
"an answering machine.\n"
"Uses LOCALSTATIONID to identify itself to the remote end.\n"
"     LOCALHEADERINFO to generate a header line on each page.\n"
"Sets REMOTESTATIONID to the receiver CSID.\n"
"Returns -1 when the user hangs up, or if the file does not exist.\n"
"Returns 0 otherwise.\n";

STANDARD_LOCAL_USER;

LOCAL_USER_DECL;

#define MAX_BLOCK_SIZE 240

static void span_message(int level, const char *msg)
{
    int ast_level;
    
    if (level == SPAN_LOG_ERROR)
        ast_level = __LOG_ERROR;
    else if (level == SPAN_LOG_WARNING)
        ast_level = __LOG_WARNING;
    else
        ast_level = __LOG_DEBUG;
    ast_log(ast_level, __FILE__, __LINE__, __PRETTY_FUNCTION__, msg);
}
/*- End of function --------------------------------------------------------*/

#if 0
static void t30_flush(t30_state_t *s, int which)
{
    //TODO:
}
/*- End of function --------------------------------------------------------*/
#endif

static void phase_e_handler(t30_state_t *s, void *user_data, int result)
{
    struct ast_channel *chan;
    char far_ident[21], result_str[32];
    
    chan = (struct ast_channel *) user_data;
    snprintf(result_str, 32, "%d", result);
    pbx_builtin_setvar_helper(chan, "PHASEESTATUS", result_str);
    if (result == T30_ERR_OK)
    {
        t30_get_far_ident(s, far_ident);
        pbx_builtin_setvar_helper(chan, "REMOTESTATIONID", far_ident);
    }
    else
    {
        ast_log(LOG_DEBUG, "==============================================================================\n");
        ast_log(LOG_DEBUG, "Fax send not successful - result (%d) %s.\n", result, t30_completion_code_to_str(result));
        ast_log(LOG_DEBUG, "==============================================================================\n");
    }
}
/*- End of function --------------------------------------------------------*/

static int txfax_exec(struct ast_channel *chan, void *data)
{
    int res = 0;
    char source_file[256];
    char *x;
    char *s;
    char *t;
    char *v;
    int option;
    int len;
    fax_state_t fax;
    int calling_party;
    int verbose;
    int samples;
    
    struct localuser *u;
    struct ast_frame *inf = NULL;
    struct ast_frame outf;

    int original_read_fmt;
    int original_write_fmt;
    
    uint8_t __buf[sizeof(uint16_t)*MAX_BLOCK_SIZE + 2*AST_FRIENDLY_OFFSET];
    uint8_t *buf = __buf + AST_FRIENDLY_OFFSET;

    if (chan == NULL)
    {
        ast_log(LOG_WARNING, "Fax transmit channel is NULL. Giving up.\n");
        return -1;
    }

    span_set_message_handler(span_message);

    /* The next few lines of code parse out the filename and header from the input string */
    if (data == NULL)
    {
        /* No data implies no filename or anything is present */
        ast_log(LOG_WARNING, "Txfax requires an argument (filename)\n");
        return -1;
    }
    
    calling_party = FALSE;
    verbose = FALSE;
    source_file[0] = '\0'; 

    for (option = 0, v = s = data;  v;  option++, s++)
    {
        t = s;
        v = strchr(s, '|');
        s = (v)  ?  v  :  s + strlen(s);
        strncpy((char *) buf, t, s - t);
        buf[s - t] = '\0';
        if (option == 0)
        {
            /* The first option is always the file name */
            len = s - t;
            if (len > 255)
                len = 255;
            strncpy(source_file, t, len);
            source_file[len] = '\0';
        }
        else if (strncmp("caller", t, s - t) == 0)
        {
            calling_party = TRUE;
        }
        else if (strncmp("debug", t, s - t) == 0)
        {
            verbose = TRUE;
        }
    }

    /* Done parsing */

    LOCAL_USER_ADD(u);

    if (chan->_state != AST_STATE_UP)
    {
        /* Shouldn't need this, but checking to see if channel is already answered
         * Theoretically asterisk should already have answered before running the app */
        res = ast_answer(chan);
    }
    
    if (!res)
    {
        original_read_fmt = chan->readformat;
        if (original_read_fmt != AST_FORMAT_SLINEAR)
        {
            res = ast_set_read_format(chan, AST_FORMAT_SLINEAR);
            if (res < 0)
            {
                ast_log(LOG_WARNING, "Unable to set to linear read mode, giving up\n");
                LOCAL_USER_REMOVE(u);
                return -1;
            }
        }
        original_write_fmt = chan->writeformat;
        if (original_write_fmt != AST_FORMAT_SLINEAR)
        {
            res = ast_set_write_format(chan, AST_FORMAT_SLINEAR);
            if (res < 0)
            {
                ast_log(LOG_WARNING, "Unable to set to linear write mode, giving up\n");
                res = ast_set_read_format(chan, original_read_fmt);
                if (res)
                    ast_log(LOG_WARNING, "Unable to restore read format on '%s'\n", chan->name);
                LOCAL_USER_REMOVE(u);
                return -1;
            }
        }
        fax_init(&fax, calling_party);
        if (verbose)
	    fax.logging.level = SPAN_LOG_SHOW_SEVERITY | SPAN_LOG_SHOW_PROTOCOL | SPAN_LOG_FLOW;

        x = pbx_builtin_getvar_helper(chan, "LOCALSTATIONID");
        if (x  &&  x[0])
            t30_set_local_ident(&fax.t30_state, x);
        x = pbx_builtin_getvar_helper(chan, "LOCALHEADERINFO");
        if (x  &&  x[0])
            t30_set_header_info(&fax.t30_state, x);
        t30_set_tx_file(&fax.t30_state, source_file, -1, -1);
        //t30_set_phase_b_handler(&fax.t30_state, phase_b_handler, chan);
        //t30_set_phase_d_handler(&fax.t30_state, phase_d_handler, chan);
        t30_set_phase_e_handler(&fax.t30_state, phase_e_handler, chan);
        t30_set_ecm_capability(&fax.t30_state, TRUE);
        t30_set_supported_compressions(&fax.t30_state, T30_SUPPORT_T4_1D_COMPRESSION | T30_SUPPORT_T4_2D_COMPRESSION | T30_SUPPORT_T6_COMPRESSION);
        while (ast_waitfor(chan, -1) > -1)
        {
            inf = ast_read(chan);
            if (inf == NULL)
            {
                res = -1;
                break;
            }
            if (inf->frametype == AST_FRAME_VOICE)
            {
                if (fax_rx(&fax, inf->data, inf->samples))
                    break;
                samples = (inf->samples <= MAX_BLOCK_SIZE)  ?  inf->samples  :  MAX_BLOCK_SIZE;
                len = fax_tx(&fax, (int16_t *) &buf[AST_FRIENDLY_OFFSET], samples);
                if (len)
                {
                    memset(&outf, 0, sizeof(outf));
                    outf.frametype = AST_FRAME_VOICE;
                    outf.subclass = AST_FORMAT_SLINEAR;
                    outf.datalen = len*sizeof(int16_t);
                    outf.samples = len;
                    outf.data = &buf[AST_FRIENDLY_OFFSET];
                    outf.offset = AST_FRIENDLY_OFFSET;
                    if (ast_write(chan, &outf) < 0)
                    {
                        ast_log(LOG_WARNING, "Unable to write frame to channel; %s\n", strerror(errno));
                        break;
                    }
                }
            }
            ast_frfree(inf);
        }
        if (inf == NULL)
        {
            ast_log(LOG_DEBUG, "Got hangup\n");
            res = -1;
        }
        if (original_read_fmt != AST_FORMAT_SLINEAR)
        {
            res = ast_set_read_format(chan, original_read_fmt);
            if (res)
                ast_log(LOG_WARNING, "Unable to restore read format on '%s'\n", chan->name);
        }
        if (original_write_fmt != AST_FORMAT_SLINEAR)
        {
            res = ast_set_write_format(chan, original_write_fmt);
            if (res)
                ast_log(LOG_WARNING, "Unable to restore write format on '%s'\n", chan->name);
        }
        t30_terminate(&fax.t30_state);
    }
    else
    {
        ast_log(LOG_WARNING, "Could not answer channel '%s'\n", chan->name);
    }
    LOCAL_USER_REMOVE(u);
    return res;
}
/*- End of function --------------------------------------------------------*/

int unload_module(void)
{
    STANDARD_HANGUP_LOCALUSERS;
    return ast_unregister_application(app);
}
/*- End of function --------------------------------------------------------*/

int load_module(void)
{
    return ast_register_application(app, txfax_exec, synopsis, descrip);
}
/*- End of function --------------------------------------------------------*/

char *description(void)
{
    return tdesc;
}
/*- End of function --------------------------------------------------------*/

int usecount(void)
{
    int res;

    STANDARD_USECOUNT(res);
    return res;
}
/*- End of function --------------------------------------------------------*/

char *key(void)
{
    return ASTERISK_GPL_KEY;
}
/*- End of function --------------------------------------------------------*/
/*- End of file ------------------------------------------------------------*/
