/* Copyright 2013-2015 Freescale Semiconductor Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * * Neither the name of the above-listed copyright holders nor the
 * names of any contributors may be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __FSL_DPDBG_H
#define __FSL_DPDBG_H

#include "fsl_dpkg.h"
#include "fsl_dpmac.h"
#include "fsl_dpni.h"

/* Data Path Debug API
 * Contains initialization APIs and runtime control APIs for DPDBG
 */

struct fsl_mc_io;

/**
 * dpdbg_open() - Open a control session for the specified object.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @dpdbg_id:	DPDBG unique ID
 * @token:	Returned token; use in subsequent API calls
 *
 * This function can be used to open a control session for an
 * already created object;
 * This function returns a unique authentication token,
 * associated with the specific object ID and the specific MC
 * portal; this token must be used in all subsequent commands for
 * this specific object
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_open(struct fsl_mc_io *mc_io,
	       uint32_t	cmd_flags,
	       int		dpdbg_id,
	       uint16_t	*token);

/**
 * dpdbg_close() - Close the control session of the object
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 *
 * After this function is called, no further operations are
 * allowed on the object without opening a new control session.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_close(struct fsl_mc_io	*mc_io,
		uint32_t		cmd_flags,
		uint16_t		token);

/**
 * struct dpdbg_attr - Structure representing DPDBG attributes
 * @id:		DPDBG object ID
 * @version:	DPDBG version
 */
struct dpdbg_attr {
	int id;
	/**
	 * struct version - Structure representing DPDBG version
	 * @major:	DPDBG major version
	 * @minor:	DPDBG minor version
	 */
	struct {
		uint16_t major;
		uint16_t minor;
	} version;
};

/**
 * dpdbg_get_attributes - Retrieve DPDBG attributes.
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @attr:	Returned object's attributes
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_get_attributes(struct fsl_mc_io	*mc_io,
			 uint32_t		cmd_flags,
			 uint16_t		token,
			 struct dpdbg_attr	*attr);

/**
 * struct dpdbg_dpni_info - Info of DPNI
 * @max_senders: Maximum number of different senders; used as the number
 *		of dedicated Tx flows; Non-power-of-2 values are rounded
 *		up to the next power-of-2 value as hardware demands it;
 *		'0' will be treated as '1'
 * @qdid: Virtual QDID.
 * @err_fqid: Virtual FQID for error queues
 * @tx_conf_fqid: Virtual FQID for global TX confirmation queue
 */
struct dpdbg_dpni_info {
	uint8_t	max_senders;
	uint32_t	qdid;
	uint32_t	err_fqid;
	uint32_t	tx_conf_fqid;
};

/**
 * dpdbg_get_dpni_info() - Retrieve info for a specific DPNI
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpni_id:	The requested DPNI ID
 * @info:	The returned info
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_get_dpni_info(struct fsl_mc_io	*mc_io,
			uint32_t		cmd_flags,
			uint16_t		token,
			int			dpni_id,
			struct dpdbg_dpni_info	*info);

/**
 * dpdbg_get_dpni_private_fqid() - Retrieve the virtual TX confirmation queue
 *					FQID of the required DPNI
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpni_id:	The requested DPNI ID
 * @sender_id:	The requested sender ID
 * @fqid:	The returned virtual private TX confirmation FQID.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_get_dpni_priv_tx_conf_fqid(struct fsl_mc_io	*mc_io,
				     uint32_t		cmd_flags,
				     uint16_t		token,
				     int		dpni_id,
				     uint8_t		sender_id,
				     uint32_t		*fqid);

/**
 * struct dpdbg_dpcon_info - Info of DPCON
 * @ch_id:	Channel ID
 */
struct dpdbg_dpcon_info {
	uint32_t	ch_id;
};

/**
 * dpdbg_get_dpcon_info() - Retrieve info of DPCON
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpcon_id:	The requested DPCON ID
 * @info:	The returned info.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_get_dpcon_info(struct fsl_mc_io		*mc_io,
			 uint32_t		cmd_flags,
			 uint16_t			token,
			 int				dpcon_id,
			 struct dpdbg_dpcon_info	*info);

/**
 * struct dpdbg_dpbp_info - Info of DPBP
 * @bpid: Virtual buffer pool ID
 */
struct dpdbg_dpbp_info {
	uint32_t	bpid;
};

/**
 * dpdbg_get_dpbp_info() - Retrieve info of DPBP
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpbp_id:	The requested DPBP ID
 * @info:	The returned info.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_get_dpbp_info(struct fsl_mc_io		*mc_io,
			uint32_t			cmd_flags,
			uint16_t			token,
			int				dpbp_id,
			struct dpdbg_dpbp_info		*info);

/**
 * dpdbg_get_dpci_fqid() - Retrieve the virtual FQID of the required DPCI
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpci_id:	The requested DPCI ID
 * @priority:	Select the queue relative to number of priorities configured at
 *		DPCI creation
 * @fqid:	The returned virtual FQID.
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_get_dpci_fqid(struct fsl_mc_io		*mc_io,
			uint32_t			cmd_flags,
			uint16_t			token,
			int				dpci_id,
			uint8_t				priority,
			uint32_t			*fqid);

/**
 * Maximum size for rule match (in bytes)
 */
#define DPDBG_MAX_RULE_SIZE		56
/**
 * Disable marking
 */
#define DPDBG_DISABLE_MARKING		0xFF

/**
 * dpdbg_prepare_ctlu_global_rule() - function prepare extract parameters
 * @dpkg_rule: defining a full Key Generation profile (rule)
 * @rule_buf: Zeroed 256 bytes of memory before mapping it to DMA
 *
 * This function has to be called before dpdbg_set_global_marking()
 */
int dpdbg_prepare_ctlu_global_rule(struct dpkg_profile_cfg	*dpkg_rule,
				   uint8_t			*rule_buf);

/**
 * struct dpdbg_rule_cfg - Rule configuration for table lookup
 * @key_iova: I/O virtual address of the key (must be in DMA-able memory)
 * @rule_iova: I/O virtual address of the rule (must be in DMA-able memory)
 * @mask_iova: I/O virtual address of the mask (must be in DMA-able memory)
 * @key_size: key and mask size (in bytes)
 */
struct dpdbg_rule_cfg {
	uint64_t	key_iova;
	uint64_t	mask_iova;
	uint64_t	rule_iova;
	uint8_t		key_size;
};

/**
 * dpdbg_set_ctlu_global_marking() - Set marking for all match rule frames
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @marking:	The requested Debug marking
 * @cfg:	Marking rule to add
 *
 * Warning: must be called after dpdbg_prepare_global_rule()
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_set_ctlu_global_marking(struct fsl_mc_io	*mc_io,
				  uint32_t		cmd_flags,
				  uint16_t		token,
				  uint8_t		marking,
				  struct dpdbg_rule_cfg	*cfg);

/**
 * All traffic classes considered
 */
#define DPDBG_DPNI_ALL_TCS	(uint8_t)(-1)
/**
 * All flows within traffic class considered
 */
#define DPDBG_DPNI_ALL_TC_FLOWS	(uint8_t)(-1)
/**
 * All buffer pools considered
 */
#define DPDBG_DPNI_ALL_DPBP	(uint8_t)(-1)

/**
 * struct dpdbg_dpni_rx_marking_cfg - Ingress frame configuration
 * @tc_id: Traffic class ID (0-7); DPDBG_DPNI_ALL_TCS for all traffic classes.
 * @flow_id: Rx flow id within the traffic class; use
 *	 'DPDBG_DPNI_ALL_TC_FLOWS' to set all flows within this tc_id;
 *	 ignored if tc_id is set to 'DPDBG_DPNI_ALL_TCS';
 * @dpbp_id: buffer pool ID; 'DPDBG_DPNI_ALL_DPBP' to set all DPBP
 * @marking: Marking for match frames;
 *		'DPDBG_DISABLE_MARKING' for disable marking
 */
struct dpdbg_dpni_rx_marking_cfg {
	uint8_t		tc_id;
	uint16_t	flow_id;
	uint16_t	dpbp_id;
	uint8_t		marking;
};

/**
 * dpdbg_set_dpni_rx_marking() - Set Rx frame marking for DPNI
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpni_id:	The requested DPNI ID
 * @cfg:	RX frame marking configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_set_dpni_rx_marking(struct fsl_mc_io			*mc_io,
			      uint32_t				cmd_flags,
			      uint16_t				token,
			      int				dpni_id,
			      struct dpdbg_dpni_rx_marking_cfg	*cfg);

/* selects global confirmation queues */
#define DPDBG_DPNI_GLOBAL_TX_CONF_QUEUE		(uint16_t)(-1)

/**
 * dpdbg_set_dpni_tx_conf_marking() - Set Tx frame marking for DPNI
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpni_id:	The requested DPNI ID
 * @sender_id:	Sender Id for the confirmation queue;
 *		'DPDBG_DPNI_GLOBAL_TX_CONF_QUEUE' for global confirmation queue
 * @marking:	The requested marking;
 *		'DPDBG_DISABLE_MARKING' for disable marking
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_set_dpni_tx_conf_marking(struct fsl_mc_io	*mc_io,
				   uint32_t		cmd_flags,
				   uint16_t		token,
				   int			dpni_id,
				   uint16_t		sender_id,
				   uint8_t		marking);

/**
 * dpdbg_set_dpio_marking() - Set debug frame marking on enqueue
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpio_id:	The requested DPIO ID
 * @marking:	The requested marking;
 *		'DPDBG_DISABLE_MARKING' for disable marking
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_set_dpio_marking(struct fsl_mc_io	*mc_io,
			   uint32_t		 cmd_flags,
			   uint16_t		 token,
			   int			 dpio_id,
			   uint8_t		 marking);

/**
 * enum dpdbg_verbosity_level - Trace verbosity level
 * @DPDBG_VERBOSITY_LEVEL_DISABLE: Trace disabled
 * @DPDBG_VERBOSITY_LEVEL_TERSE: Terse trace
 * @DPDBG_VERBOSITY_LEVEL_VERBOSE: Verbose trace
 */
enum dpdbg_verbosity_level {
	DPDBG_VERBOSITY_LEVEL_DISABLE = 0,
	DPDBG_VERBOSITY_LEVEL_TERSE,
	DPDBG_VERBOSITY_LEVEL_VERBOSE
};

/**
 * dpdbg_set_ctlu_global_trace() - Set global trace configuration for CTLU trace
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @cfg:	trace rule to add
 *
 * Warning: must be called after dpdbg_prepare_global_rule()
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_set_ctlu_global_trace(struct fsl_mc_io	*mc_io,
				uint32_t		cmd_flags,
				uint16_t		token,
				struct dpdbg_rule_cfg	*cfg);

/**
 * Number of DPIO trace points
 */
#define DPDBG_NUM_OF_DPIO_TRACE_POINTS	2

/**
 * enum dpdbg_dpio_trace_type - Define Trace point type
 * @DPDBG_DPIO_TRACE_TYPE_ENQUEUE: This trace point triggers when an enqueue
 *				command, received via this portal,
 *				and containing a marked frame, is executed
 * @DPDBG_DPIO_TRACE_TYPE_DEFERRED: This trace point triggers when the deferred
 *				enqueue of a marked frame received via this
 *				portal completes
 */
enum dpdbg_dpio_trace_type {
	DPDBG_DPIO_TRACE_TYPE_ENQUEUE = 0,
	DPDBG_DPIO_TRACE_TYPE_DEFERRED = 1
};

/**
 * struct dpdbg_dpio_trace_cfg - Configure the behavior of a trace point
 *			when a frame marked with the specified DD code point is
 *			encountered
 * @marking:	  this field will be written into the DD field of every FD
 *		  enqueued in this DPIO.
 *		  'DPDBG_DISABLE_MARKING' for disable marking
 * @verbosity:	  Verbosity level
 * @enqueue_type: Enqueue trace point type defining a full Key Generation
 *		  profile (rule)
 */
struct dpdbg_dpio_trace_cfg {
		uint8_t						marking;
		enum dpdbg_verbosity_level	verbosity;
		enum dpdbg_dpio_trace_type	enqueue_type;
};

/**
 * dpdbg_set_dpio_trace() - Set trace for DPIO for every enqueued frame to
 *							the portal
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpio_id:	The requested DPIO ID
 * @trace_point: Trace points configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_set_dpio_trace(struct fsl_mc_io	*mc_io,
			 uint32_t		cmd_flags,
			 uint16_t		token,
			 int			dpio_id,
			 struct dpdbg_dpio_trace_cfg
				 trace_point[DPDBG_NUM_OF_DPIO_TRACE_POINTS]);

/**
 * struct dpdbg_dpni_trace_cfg - Configure the behavior of a trace point when a
 * @tc_id: Traffic class ID (0-7); DPDBG_DPNI_ALL_TCS for all traffic classes.
 * @flow_id: Rx flow id within the traffic class; use
 *	 'DPDBG_DPNI_ALL_TC_FLOWS' to set all flows within this tc_id;
 *	 ignored if tc_id is set to 'DPDBG_DPNI_ALL_TCS';
 * @dpbp_id: buffer pool ID; 'DPDBG_DPNI_ALL_DPBP' to set all DPBP
 * @marking: Marking for match frames;
 *		'DPDBG_DISABLE_MARKING' for disable marking
 */
struct dpdbg_dpni_rx_trace_cfg {
	uint8_t		tc_id;
	uint16_t	flow_id;
	uint16_t	dpbp_id;
	uint8_t		marking;
};

/**
 * dpdbg_set_dpni_rx_trace() - Set trace for DPNI ingress (WRIOP ingress).
 *		in case of multiple requests for different DPNIs - the trace
 *		will be for the latest DPNI requested.
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpni_id:	The requested DPNI ID
 * @trace_cfg:  Trace configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_set_dpni_rx_trace(struct fsl_mc_io			*mc_io,
			    uint32_t				cmd_flags,
			    uint16_t				token,
			    int					dpni_id,
			    struct dpdbg_dpni_rx_trace_cfg	*trace_cfg);

/**
 * All DPNI senders
 */
#define DPDBG_DPNI_ALL_SENDERS	(uint16_t)(-1)

/**
 * struct dpdbg_dpni_trace_cfg - Configure the behavior of a trace point when a
 *		frame marked with the specified DD code point is encountered
 * @marking: The requested debug marking;
 *		'DPDBG_DISABLE_MARKING' for disable marking
 */
struct dpdbg_dpni_tx_trace_cfg {
		uint8_t marking;
};

/**
 * dpdbg_set_dpni_tx_trace() - Set trace for DPNI dequeued frames
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpni_id:	The requested DPNI ID
 * @sender_id:	Sender ID; 'DPDBG_DPNI_ALL_SENDERS' for all senders
 * @trace_cfg:	Trace configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_set_dpni_tx_trace(struct fsl_mc_io			*mc_io,
			    uint32_t				cmd_flags,
			    uint16_t				token,
			    int					dpni_id,
			    uint16_t				sender_id,
			    struct dpdbg_dpni_tx_trace_cfg	*trace_cfg);

/**
 * Number of DPCON trace points
 */
#define DPDBG_NUM_OF_DPCON_TRACE_POINTS	2

/**
 * struct dpdbg_dpcon_trace_cfg - Configure the behavior of a trace point when a
 *		frame marked with the specified DD code point is encountered
 * @marking: The requested debug marking;
 *		'DPDBG_DISABLE_MARKING' for disable marking
 * @verbosity: Verbosity level
 */
struct dpdbg_dpcon_trace_cfg {
		uint8_t			marking;
		enum dpdbg_verbosity_level	verbosity;
};

/**
 * dpdbg_set_dpcon_trace() - Set trace for DPCON when a frame marked with a
 *				specified marking is dequeued from a WQ in the
 *				channel selected
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpcon_id:	The requested DPCON ID
 * @trace_point: Trace points configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_set_dpcon_trace(struct fsl_mc_io		*mc_io,
			  uint32_t			cmd_flags,
			  uint16_t			token,
			  int				dpcon_id,
			  struct dpdbg_dpcon_trace_cfg
				  trace_point[DPDBG_NUM_OF_DPCON_TRACE_POINTS]);

/**
 * Number of DPSECI trace points
 */
#define DPDBG_NUM_OF_DPSECI_TRACE_POINTS	2

/**
 * struct dpdbg_dpseci_trace_cfg - Configure the behavior of a trace point when
 *			 a frame marked with the specified DD code point is
 *			 encountered
 * @marking: The requested debug marking;
 *		'DPDBG_DISABLE_MARKING' for disable marking
 * @verbosity: Verbosity level
 */
struct dpdbg_dpseci_trace_cfg {
		uint8_t			marking;
		enum dpdbg_verbosity_level	verbosity;
};

/**
 * dpdbg_set_dpseci_trace() - Set trace for DPSECI when a frame marked with the
 *				specific marking is enqueued via this portal.
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpseci_id:	The requested DPSECI ID
 * @trace_point: Trace points configuration
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_set_dpseci_trace(struct fsl_mc_io	*mc_io,
			   uint32_t		cmd_flags,
			   uint16_t		token,
			   int			dpseci_id,
			   struct dpdbg_dpseci_trace_cfg
				trace_point[DPDBG_NUM_OF_DPSECI_TRACE_POINTS]);

/**
 * dpdbg_get_dpmac_counter() - DPMAC packet throughput
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpmac_id:	The requested DPMAC ID
 * @counter_type:   The requested DPMAC counter
 * @counter:	Returned counter value
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_get_dpmac_counter(struct fsl_mc_io		*mc_io,
			    uint32_t			cmd_flags,
			    uint16_t			token,
			    int			dpmac_id,
			    enum dpmac_counter		counter_type,
			    uint64_t			*counter);

/**
 * dpdbg_get_dpni_counter() - DPNI packet throughput
 *
 * @mc_io:	Pointer to MC portal's I/O object
 * @cmd_flags:	Command flags; one or more of 'MC_CMD_FLAG_'
 * @token:	Token of DPDBG object
 * @dpni_id:	The requested DPNI ID
 * @counter_type:   The requested DPNI counter
 * @counter:	Returned counter value
 *
 * Return:	'0' on Success; Error code otherwise.
 */
int dpdbg_get_dpni_counter(struct fsl_mc_io	*mc_io,
			   uint32_t		cmd_flags,
			   uint16_t		token,
			   int			dpni_id,
			   enum dpni_counter	counter_type,
			   uint64_t		*counter);

#endif /* __FSL_DPDBG_H */
