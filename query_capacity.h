/* Copyright IBM Corp. 2013, 2015 */

/** @file */

#ifndef QUERY_CAPACITY
#define QUERY_CAPACITY


/** \enum qc_attr_id
 * Defines the attributes retrievable by the API. Note that an attribute can
 * exist for multiple layers. Also, attributes will only be valid if retrieved
 * as the correct type.
 *
 * The following tables detail which attributes of what types are available for
 * what layers. The letter encoding in the 'Src' column describes how the
 * value is gained:
 *   - S  stands for attempting extraction through \c /proc/sysinfo
 *   - H  stands for attempting extraction through hypfs (usually
 *        \c /sys/hypervisor/s390/)
 *   - V  stands for attempting extraction through *VMINFO
 *
 * Several letters indicate the order in which the value is attempted to be
 * acquired. If the extraction of the value in a later phase succeeds, it will
 * overwrite the value acquired in an earlier phase. If the extraction of the
 * value in a later phase does not succeed, it will not dismiss the existing
 * value, if a previous phase has set it already.
 *
 * All values from the \c "S" and \c "H" sources are present as soon as \c /proc/sysinfo
 * and hypfs respectively can be accessed, which is most likely and usually the case.
 * The \c "V" fields depend on availability as indicated by VM.
 *
 * For platform agnostic processing (not caring about what layers are
 * present in the system), it is recommended to walk through all layers,
 *  - processing #qc_num_cpu_total, #qc_num_cpu_dedicated, and #qc_num_cpu_shared, if the
 *    layer is of the category \c "HOST" or \c "GUEST", and
 *  - process pool capping values, if the layer is in the \c "POOL" category.
 * Then find the minimal number of the stack for a single virtual server; for
 * running on multiple virtual servers, use #qc_layer_name to identify layers
 * (since several higher layer entities could run on the same lower layer entity).
 * Using only said attributes should result in a high probability of
 * good results -- those attributes should be presented by all layers and
 * provided by all sane setups.
 *
 * Notes:
 * - A z/VM-guest running in a z/VM-CPU-pool is a layer higher than the z/VM-CPU-pool
 * - All strings (char pointers) carry the trailing zero byte.
 * - See #qc_attr_id for general explanation of attributes, and the \c 'Comment' column
 *   for layer-specific considerations
 *
 * Attributes for CECs (layer 0)       | Type | Src | Comment
 * ------------------------------------|------|-----|-------------------------------------
 * #qc_layer_type_num                  | int  |     | Hardcoded to \c QC_LAYER_TYPE_CEC
 * #qc_layer_category_num              | int  |     | Hardcoded to \c QC_LAYER_CAT_HOST
 * #qc_layer_type                      |string|     | Hardcoded to \c "CEC"
 * #qc_layer_category                  |string|     | Hardcoded to \c "HOST"
 * #qc_layer_name                      |string|<CODE>&nbsp;&nbsp;V</CODE>| CPC name of machine
 * #qc_manufacturer                    |string|<CODE>S&nbsp;V</CODE>| \n
 * #qc_type                            |string|<CODE>S&nbsp;V</CODE>| \n
 * #qc_model_capacity                  |string|<CODE>S&nbsp;&nbsp;</CODE>| \n
 * #qc_model                           |string|<CODE>S&nbsp;&nbsp;</CODE>| \n
 * #qc_sequence_code                   |string|<CODE>S&nbsp;V</CODE>| \n
 * #qc_plant                           |string|<CODE>S&nbsp;V</CODE>| \n
 * #qc_num_cpu_total                   | int  |<CODE>S</CODE>| Sum of #qc_num_cpu_configured, #qc_num_cpu_reserved and #qc_num_cpu_standby, or #qc_num_cpu_dedicated and #qc_num_cpu_shared.<br>Note that sum of #qc_num_cp_total and #qc_num_ifl_total might be smaller, since assist processors are not accounted
 * #qc_num_cpu_configured              | int  |<CODE>S&nbsp;&nbsp;</CODE>| General purpose CPUs only without IFLs
 * #qc_num_cpu_standby                 | int  |<CODE>S&nbsp;&nbsp;</CODE>| General purpose CPUs which are in the (very brief) process of being added to the configuration
 * #qc_num_cpu_reserved                | int  |<CODE>S&nbsp;&nbsp;</CODE>| IFLs, zIIPs, spares, excluding IFPs (Internal Firmware Processors)
 * #qc_num_cpu_dedicated               | int  |<CODE>&nbsp;&nbsp;V</CODE>| Note that sum of #qc_num_cp_dedicated and #qc_num_ifl_dedicated might be smaller, since assist processors are not accounted
 * #qc_num_cpu_shared                  | int  |<CODE>&nbsp;&nbsp;V</CODE>| Note that sum of #qc_num_cp_shared and #qc_num_ifl_shared might be smaller, since assist processors are not accounted
 * #qc_num_cp_total                    | int  |<CODE>&nbsp;&nbsp;V</CODE>| Total number of CPs (general purpose CP, equals the sum of shared and dedicated CPs
 * #qc_num_cp_dedicated                | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_num_cp_shared                   | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_num_ifl_total                   | int  |<CODE>&nbsp;&nbsp;V</CODE>| Equals the sum of #qc_num_ifl_dedicated and #qc_num_ifl_shared in layer
 * #qc_num_ifl_dedicated               | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_num_ifl_shared                  | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_capability                      | float|<CODE>S&nbsp;&nbsp;</CODE>| \n
 * #qc_secondary_capability            | float|<CODE>S&nbsp;&nbsp;</CODE>| \n
 * #qc_capacity_adjustment_indication  | int  |<CODE>S&nbsp;&nbsp;</CODE>| \n
 * #qc_capacity_change_reason          | int  |<CODE>S&nbsp;&nbsp;</CODE>| \n
 *
 * Attributes for LPARs (layer 1)      | Type | Src | Comment
 * ------------------------------------|------|-----|-------------------------------------
 * #qc_layer_type_num                  | int  |     | Hardcoded to \c QC_LAYER_TYPE_LPAR
 * #qc_layer_category_num              | int  |     | Hardcoded to \c QC_LAYER_CAT_GUEST
 * #qc_layer_type                      |string|     | Hardcoded to \c "LPAR"
 * #qc_layer_category                  |string|     | Hardcoded to \c "GUEST"
 * #qc_layer_name                      |string|<CODE>S&nbsp;V</CODE>| Name of LPAR
 * #qc_partition_number                | int  |<CODE>S&nbsp;&nbsp;</CODE>| \n
 * #qc_partition_char                  |string|<CODE>S&nbsp;&nbsp;</CODE>| \n
 * #qc_partition_char_num              | int  |<CODE>S&nbsp;&nbsp;</CODE>| \n
 * #qc_adjustment                      | int  |<CODE>S&nbsp;&nbsp;</CODE>| Adjustment factor of LPAR
 * #qc_num_cpu_total                   | int  |<CODE>S</CODE>| Sum of #qc_num_cpu_configured, #qc_num_cpu_standby and #qc_num_cpu_reserved, or #qc_num_cpu_dedicated and #qc_num_cpu_shared
 * #qc_num_cpu_configured              | int  |<CODE>S&nbsp;&nbsp;</CODE>| \n
 * #qc_num_cpu_standby                 | int  |<CODE>S&nbsp;&nbsp;</CODE>| General purpose CPUs that require add'l configuration within the LPAR image to become usable
 * #qc_num_cpu_reserved                | int  |<CODE>S&nbsp;&nbsp;</CODE>| General purpose CPUs that require add'l interaction by the LPAR's administrator to become usable
 * #qc_num_cpu_dedicated               | int  |<CODE>S&nbsp;V</CODE>| Note that sum of #qc_num_cp_dedicated and #qc_num_ifl_dedicated might be smaller, since assist processors are not accounted
 * #qc_num_cpu_shared                  | int  |<CODE>S&nbsp;V</CODE>| Note that sum of #qc_num_cp_shared and #qc_num_ifl_shared might be smaller, since assist processors are not accounted
 * #qc_num_cp_total                    | int  |<CODE>&nbsp;H</CODE>| Sum of #qc_num_cp_dedicated and #qc_num_cp_shared
 * #qc_num_cp_dedicated                | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_num_cp_shared                   | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_num_ifl_total                   | int  |<CODE>&nbsp;H</CODE>| Sum of #qc_num_ifl_dedicated and #qc_num_ifl_shared
 * #qc_num_ifl_dedicated               | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_num_ifl_shared                  | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_cp_absolute_capping             | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_ifl_absolute_capping            | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_cp_weight_capping               | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_ifl_weight_capping              | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 *
 *
 * Attributes for z/VM hypervisors     | Type | Src | Comment
 * ------------------------------------|------|-----|-------------------------------------
 * #qc_layer_type_num                  | int  |     | Hardcoded to \c QC_LAYER_TYPE_ZVM_HYPERVISOR
 * #qc_layer_category_num              | int  |     | Hardcoded to \c QC_LAYER_CAT_HOST
 * #qc_layer_type                      |string|     | Hardcoded to \c "z/VM-hypervisor"
 * #qc_layer_category                  |string|     | Hardcoded to \c "HOST"
 * #qc_layer_name                      |string|<CODE>&nbsp;&nbsp;V</CODE>| System identifier of the hypervisor
 * #qc_cluster_name                    |string|<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_control_program_id              |string|<CODE>S&nbsp;&nbsp;</CODE>| ID of CP
 * #qc_adjustment                      | int  |<CODE>S&nbsp;&nbsp;</CODE>| Adjustment factor of z/VM
 * #qc_hardlimit_consumption           | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_num_cpu_total                   | int  |<CODE>&nbsp;&nbsp;V</CODE>| Sum of #qc_num_cpu_dedicated and #qc_num_cpu_shared.<br>Note that sum of #qc_num_cp_total and #qc_num_ifl_total might be smaller, since assist processors are not accounted
 * #qc_num_cpu_dedicated               | int  |<CODE>&nbsp;&nbsp;V</CODE>| Note that sum of #qc_num_cp_dedicated and #qc_num_ifl_dedicated might be smaller, since assist processors are missing
 * #qc_num_cpu_shared                  | int  |<CODE>&nbsp;&nbsp;V</CODE>| Note that sum of #qc_num_cp_shared and #qc_num_ifl_shared might be smaller, since assist processors are missing
 * #qc_num_cp_total                    | int  |<CODE>&nbsp;&nbsp;V</CODE>| Sum of #qc_num_cp_dedicated and #qc_num_cp_shared
 * #qc_num_cp_dedicated                | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_num_cp_shared                   | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_num_ifl_total                   | int  |<CODE>&nbsp;&nbsp;V</CODE>| Sum of #qc_num_ifl_dedicated and #qc_num_ifl_shared
 * #qc_num_ifl_dedicated               | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_num_ifl_shared                  | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 *
 *
 * Attributes for z/VM CPU pools       | Type | Src | Comment
 * ------------------------------------|------|-----|-------------------------------------
 * #qc_layer_type_num                  | int  |     | Hardcoded to \c QC_LAYER_TYPE_ZVM_CPU_POOL
 * #qc_layer_category_num              | int  |     | Hardcoded to \c QC_LAYER_CAT_POOL
 * #qc_layer_type                      |string|     | Hardcoded to \c "z/VM-CPU-pool"
 * #qc_layer_category                  |string|     | Hardcoded to \c "POOL"
 * #qc_layer_name                      |string|<CODE>&nbsp;&nbsp;V</CODE>| Name of CPU pool
 * #qc_cp_limithard_cap                | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_cp_capacity_cap                 | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_cp_capped_capacity              | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_ifl_limithard_cap               | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_ifl_capacity_cap                | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_ifl_capped_capacity             | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 *
 *
 * Attributes for z/VM guests           | Type | Src | Comment
 * ------------------------------------|------|-----|-------------------------------------
 * #qc_layer_type_num                  | int  |     | Hardcoded to \c QC_LAYER_TYPE_ZVM_GUEST
 * #qc_layer_category_num              | int  |     | Hardcoded to \c QC_LAYER_CAT_GUEST
 * #qc_layer_type                      |string|     | Hardcoded to \c "z/VM-guest"
 * #qc_layer_category                  |string|     | Hardcoded to \c "GUEST"
 * #qc_layer_name                      |string|<CODE>S&nbsp;V</CODE>| Userid of guest
 * #qc_capping                         |string|<CODE>&nbsp;H</CODE>| \n
 * #qc_capping_num                     | int  |<CODE>&nbsp;H</CODE>| \n
 * #qc_num_cpu_total                   | int  |<CODE>S&nbsp;V</CODE>| Sum of #qc_num_cpu_configured, #qc_num_cpu_standby and #qc_num_cpu_reserved, or #qc_num_cpu_dedicated and #qc_num_cpu_shared<br>Note that sum of #qc_num_cp_total and #qc_num_ifl_total might be smaller, since assist processors are not accounted
 * #qc_num_cpu_configured              | int  |<CODE>S&nbsp;&nbsp;</CODE>| \n
 * #qc_num_cpu_standby                 | int  |<CODE>S&nbsp;&nbsp;</CODE>| \n
 * #qc_num_cpu_reserved                | int  |<CODE>S&nbsp;&nbsp;</CODE>| \n
 * #qc_num_cpu_dedicated               | int  |<CODE>&nbsp;HV</CODE>| Note that sum of #qc_num_cp_dedicated and #qc_num_ifl_dedicated might be smaller, since assist processors are not accounted
 * #qc_num_cpu_shared                  | int  |<CODE>&nbsp;HV</CODE>| Note that sum of #qc_num_cp_shared and #qc_num_ifl_shared might be smaller, since assist processors are not accounted
 * #qc_num_cp_total                    | int  |<CODE>&nbsp;&nbsp;V</CODE>| Sum of #qc_num_cp_dedicated and #qc_num_cp_shared
 * #qc_num_cp_dedicated                | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_num_cp_shared                   | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_num_ifl_total                   | int  |<CODE>&nbsp;&nbsp;V</CODE>| Sum of #qc_num_ifl_dedicated and #qc_num_ifl_shared
 * #qc_num_ifl_dedicated               | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_num_ifl_shared                  | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_mobility_eligible               | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_has_multiple_cpu_types          | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_cp_dispatch_limithard           | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_cp_dispatch_type                | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_cp_capped_capacity              | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_ifl_dispatch_limithard          | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 * #qc_ifl_dispatch_type               | int  |<CODE>&nbsp;&nbsp;V</CODE>| Only set in presence of IFLs\n
 * #qc_ifl_capped_capacity             | int  |<CODE>&nbsp;&nbsp;V</CODE>| \n
 *
 *
 * Attributes for KVM hypervisors      | Type | Src | Comment
 * ------------------------------------|------|-----|-------------------------------------
 * #qc_layer_type_num                  | int  |     | Hardcoded to \c QC_LAYER_TYPE_KVM_HYPERVISOR
 * #qc_layer_category_num              | int  |     | Hardcoded to \c QC_LAYER_CAT_HOST
 * #qc_layer_type                      |string|     | Hardcoded to \c "KVM-hypervisor"
 * #qc_layer_category                  |string|     | Hardcoded to \c "HOST"
 * #qc_control_program_id              |string|<CODE>S&nbsp;&nbsp;</CODE>| Host ID
 * #qc_adjustment                      | int  |<CODE>S&nbsp;&nbsp;</CODE>| adjustment factor of VM
 *
 *
 * Attributes for KVM guests           | Type | Src | Comment
 * ------------------------------------|------|-----|-------------------------------------
 * #qc_layer_type_num                  | int  |     | Hardcoded to \c QC_LAYER_TYPE_KVM_GUEST
 * #qc_layer_category_num              | int  |     | Hardcoded to \c QC_LAYER_CAT_GUEST
 * #qc_layer_type                      |string|     | Hardcoded to \c "KVM-guest"
 * #qc_layer_category                  |string|     | Hardcoded to \c "GUEST"
 * #qc_layer_name                      |string|<CODE>S&nbsp;&nbsp;</CODE>| Guest name truncated to 8 characters [1]
 * #qc_layer_extended_name             |string|<CODE>S&nbsp;&nbsp;</CODE>| Guest name with up to 256 characters [1]
 * #qc_layer_uuid                      |string|<CODE>S&nbsp;&nbsp;</CODE>| Guest's universal unique ID
 * #qc_num_cpu_total                   | int  |<CODE>S&nbsp;&nbsp;</CODE>| Sum of #qc_num_cpu_configured, #qc_num_cpu_standby and #qc_num_cpu_reserved
 * #qc_num_cpu_configured              | int  |<CODE>S&nbsp;&nbsp;</CODE>| \n
 * #qc_num_cpu_standby                 | int  |<CODE>S&nbsp;&nbsp;</CODE>| \n
 * #qc_num_cpu_reserved                | int  |<CODE>S&nbsp;&nbsp;</CODE>| \n
 *
 * [1] Available starting with RHEL7.2 and SLES12SP1<br>
 * [2] <I>z/Architecture Principles of Operation</I>, SA22-7832
 * [3] <I>z/VM: CP Commands and Utilities Reference</I>, SC24-6175
 */
enum qc_layer_types {
	/** CEC */
	QC_LAYER_TYPE_CEC = 1,
	/** LPAR */
	QC_LAYER_TYPE_LPAR = 2,
	/** z/VM Hypervisor */
	QC_LAYER_TYPE_ZVM_HYPERVISOR = 3,
	/** z/VM CPU Pool */
	QC_LAYER_TYPE_ZVM_CPU_POOL = 4,
	/** z/VM Guest */
	QC_LAYER_TYPE_ZVM_GUEST = 5,
	/** KVM Hypervisor */
	QC_LAYER_TYPE_KVM_HYPERVISOR = 6,
	/** KVM Guest */
	QC_LAYER_TYPE_KVM_GUEST = 7
};

/** \enum qc_layer_categories
 * Layer categories. */
enum qc_layer_categories {
	/** Layer category for guests, namely LPARs, z/VM and KVM guests */
	QC_LAYER_CAT_GUEST = 1,
	/** Layer category for hosts, namely CEC, z/VM and KVM hosts  */
	QC_LAYER_CAT_HOST = 2,
	/** Layer category for pools (currently z/VM Pools only) */
	QC_LAYER_CAT_POOL = 3
};

/** \enum qc_part_chars
 * Characteristic of an an LPAR. */
enum qc_part_chars {
	/** LPAR has dedicated resources */
	QC_PART_CHAR_DEDICATED = 1,
	/** LPAR shares resources with other LPARs */
	QC_PART_CHAR_SHARED = 2,
	/** LPAR has limited resources */
	QC_PART_CHAR_LIMITED = 4
};

/** \enum qc_cappings
 * Numeric representation of the capping type, see #qc_capping. */
enum qc_cappings {
	/** Capping turned off */
	QC_CAPPING_OFF = 0,
	/** Soft capping */
	QC_CAPPING_SOFT = 1,
	/** Hard capping */
	QC_CAPPING_HARD = 2
};


/** \enum qc_attr_id */
enum qc_attr_id {
	/** Adjustment factor, see layer tables for details */
	qc_adjustment = 0,
	/** Capability rating, see \c STSI instruction in [2] */
	qc_capability = 1,
	/** Capacity adjustment value, see \c STSI instruction in [2] */
 	qc_capacity_adjustment_indication = 2,
 	/** Reason for capacity adjustment, see \c STSI instruction in [2] */
	qc_capacity_change_reason = 3,
	/** Capping type: \c "off", \c "soft", \c "hard" */
	qc_capping = 4,
	/** Numeric representation of capping type, see enum #qc_cappings */
	qc_capping_num = 5,
	/** SSI name if part of SSI cluster */
	qc_cluster_name = 6,
	/** ID of control program */
	qc_control_program_id = 7,
	/** CP absolute capping value. Scaled value where 0x10000 equals one CPU, or 0 if no capping set */
	qc_cp_absolute_capping = 8,
	/** 1 if pool's CP virtual type has capacity capping<BR> 0 if not<br>See \c DEFINE \c CPUPOOL command in [3] */
	qc_cp_capacity_cap = 9,
	/** Guest current capped capacity for shared virtual CPs -- scaled value where 0x10000 equals to one CPU, or 0 if no capping set */
	qc_cp_capped_capacity = 10,
	/** 1 if guest CP dispatch type has LIMITHARD cap.,<BR> 0 if not<br>See \c SET \c SRM command in [3] */
	qc_cp_dispatch_limithard = 11,
	/** Dispatch type for guest CPs:<br>00=General Purpose (CP) */
	qc_cp_dispatch_type = 12,
	/** 1 if pool's CP virtual type has limithard capping<BR> 0 if not<br>See \c DEFINE \c CPUPOOL command in [3] */
	qc_cp_limithard_cap = 13,
	/** CP weight-based capping value -- scaled value where 0x10000 equals one CPU, or 0 if no capping set */
	qc_cp_weight_capping = 14,
	/** 1 if SRM hardlimit setting is consumption<BR> 0 if deadline<br>See \c SET \c SRM command in [3] */
	qc_hardlimit_consumption = 15,
	/** 1 if guest has multiple CPU types,<BR> 0 if not */
	qc_has_multiple_cpu_types = 16,
	/** IFL absolute capping value -- scaled value where 0x10000 equals one CPU, or 0 if no capping set */
	qc_ifl_absolute_capping = 17,
	/** 1 if pool's IFL virtual type has capacity capping<BR> 0 if not<br>See \c DEFINE \c CPUPOOL command in [3] */
	qc_ifl_capacity_cap = 18,
	/** Guest current capped capacity for shared virtual IFLs -- scaled value where 0x10000 equals to one CPU, or 0 if no capping set */
	qc_ifl_capped_capacity = 19,
	/** 1 if guest IFL dispatch type has LIMITHARD cap.,<BR> 0 if not<br>See \c SET \c SRM command in [3] */
	qc_ifl_dispatch_limithard = 20,
	/** Dispatch type for guest CPs:<BR> 00=General Purpose (CP),<BR> 03=IFL */
	qc_ifl_dispatch_type = 21,
	/** 1 if pool's IFL virtual type has limithard capping<BR> 0 if not<br>See \c DEFINE \c CPUPOOL command in [3] */
	qc_ifl_limithard_cap = 22,
	/** IFL weight-based capping value -- scaled value where 0x10000 equals one CPU, or 0 if no capping set */
	qc_ifl_weight_capping = 23,
	/** Layer category, see layer tables above for details */
	qc_layer_category = 24,
	/** Numeric representation  of layer category, see enum #qc_layer_categories */
	qc_layer_category_num = 25,
	/** Guest extended name */
	qc_layer_extended_name = 26,
	/** Name of container, see layer tables for details */
	qc_layer_name = 27,
	/** Layer type, see layer tables above for details */
	qc_layer_type = 28,
	/** Numeric representation  of layer type, see enum #qc_layer_types */
	qc_layer_type_num = 29,
	/** Guest's universal unique ID */
	qc_layer_uuid = 30,
	/** Company that manufactured box */
	qc_manufacturer = 31,
	/** 1 if guest is eligible for mobility,<BR> 0 if not */
	qc_mobility_eligible = 32,
	/** Model identifier, see \c STSI instruction in [2] */
	qc_model = 33,
	/** Model capacity of machine, see \c STSI instruction in [2] */
	qc_model_capacity = 34,
	/** Sum of dedicated CPs in layer */
	qc_num_cp_dedicated = 35,
	/** Sum of shared CPs  in layer */
	qc_num_cp_shared = 36,
	/** Sum of all CPs in layer */
	qc_num_cp_total = 37,
	/** Sum of configured CPs and IFLs in layer */
	qc_num_cpu_configured = 38,
	/** Sum of dedicated CPs and IFLs in layer */
	qc_num_cpu_dedicated = 39,
	/** Sum of reserved CPs and IFLs in layer */
	qc_num_cpu_reserved = 40,
	/** Sum of shared CPs and IFLs in layer */
	qc_num_cpu_shared = 41,
	/** Sum of standby CPs and IFLs in layer */
	qc_num_cpu_standby = 42,
	/** Sum of all CPs and IFLs in layer */
	qc_num_cpu_total = 43,
	/** Sum of dedicated IFLs in layer */
	qc_num_ifl_dedicated = 44,
	/** Sum of shared IFLs in layer */
	qc_num_ifl_shared = 45,
	/** Sum of all IFLs (Integrated Facility for Linux) in layer */
	qc_num_ifl_total = 46,
	/** Partition characteristics, any combination of \c "Dedicated", \c "Shared" and \c "Limited", also see \c STSI instruction in [2] */
	qc_partition_char = 47,
	/** Numeric representation of partition characteristics, see enum #qc_part_chars */
	qc_partition_char_num = 48,
	/** Partition number, see \c STSI instruction in [2] */
	qc_partition_number = 49,
	/** Identifier of the manufacturing plant, see \c STSI instruction in [2] */
	qc_plant = 50,
	/** Secondary capability rating, see \c STSI instruction in [2] */
	qc_secondary_capability = 51,
	/** Sequence code of machine, see \c STSI instruction in [2] */
	qc_sequence_code = 52,
	/** 4-digit machine type */
	qc_type = 53,
};


/**
 * Attaches to system information sources and prepares the extraction of
 * system information. Some information may be gathered at this time
 * already: under LPAR, \c /proc/sysinfo is read and interpreted; under z/VM,
 * *VMINFO data is read using the \c STHYI instruction (requires z/VM 6.3 with
 * APAR VM65419, or higher). Capacity queries then take place based on this
 * information which could be considered cached.<BR>
 * Memory will be allocated in this function, which has to be released by
 * closing the configuration again. While a configuration is open, SSI
 * migration in z/VM is not blocked and can occur. In case a migration occurs
 * after a configuration has been opened, closing the configuration and
 * re-opening it ensures capacity information is used from the migrated-to
 * system.<BR>
 * Use the following environment variables to operate built-in service facilities:
 * - \c QC_DEBUG: Set to an integer value
 *   - >0 to enable logging to a file \c /tmp/qclib-XXXXXX.
 *   - >1 to have data dumped to a directory named \c /tmp/qclib-XXXXXX.dump-XXX
 *     on every qc_open() call.<br>
 *  To disable logging, either see qc_close(), or set \c QC_DEBUG to a value
 *   <=0 on the next qc_open() call.<BR>
 * - \c QC_AUTODUMP: Set to a value >0 to trigger a dump to a directory named
 *   \c /tmp/qclib-XXXXXX.dump-XXX if an error is encountered withing qc_open().<br>
 *   Note: This will also create an empty log file for technical reasons,
 *   unless \c QC_DEBUG was set to a value >0<BR>
 * - \c QC_USE_DUMP: To run with a previous dump instead of live data, point this
 *   environment variable to a directory containing the dump data.
 *
 * @see qc_close()
 *
 * @param rc Return parameter indicating the return code. Set to
 * - 0 on success,
 * - <0 in case of an error, and
 * - >0 if the configuration could not be read completely at the moment,
 *   but a retry later on could provide the missing data.
 * @return Returns a configuration handle which is valid for reading out
 *         capacity data until the configuration is closed. Returns NULL in
 *         case of an error.
 */
void *qc_open(int *rc);

/**
 * Closes the configuration handle and releases all memory allocated when the
 * configuration was opened. The configuration handle is invalid after
 * calling this function, as are any returned pointers of previous capacity
 * function calls.
 *
 * If logging or autodumping was enabled on qc_open(), environment variables
 * \c QC_DEBUG and \c QC_AUTODUMP need to be set to integers <=0 on the final
 * call to qc_close() (or whenever neither functionality is not required
 * anymore) to correctly free up all resources.
 *
 * @param hdl Handle of the configuration to close.
 */
void qc_close(void *hdl);

/**
 * Get the number of layers.
 *
 * @param hdl Handle of the configuration to use.
 * @param rc Return parameter indicating the return code. Set to
 * - 0 on success,
 * - <0 in case of an error.
 * @return Number of layer. E.g.
 * - 2 if Linux runs in an LPAR
 * - 4 if a z/VM guest runs in a z/VM Hypervisor in an LPAR, or
 * - 5 if a z/VM guest runs in a capping group in z/VM running in LPAR.
 */
int qc_get_num_layers(void *hdl, int *rc);

/**
 * Returns the attribute of type string designated by \p id. If the attribute is
 * not available at the specified layer, the attribute is not of type string,
 * or another error occured, return parameter \p valid will be set accordingly.
 *
 * @see qc_get_attribute_int()
 * @see qc_get_attribute_float()
 *
 * @param hdl Handle of the configuration to use.
 * @param id Attribute to retrieve.
 * @param layer Specifies the layer, e.g.
 * - 0: CEC layer information,
 * - 1: LPAR layer information, etc.
 * @param value Return parameter returning the string attribute's value or NULL
 * in case of an error.
 @return Indicating validity of the queried attribute as follows:
 * - >0  attribute is valid
 * -  0  attribute exists but is not set
 * - <0  an error occurred retrieving the attribute
 */
int qc_get_attribute_string(void *hdl, enum qc_attr_id id, int layer, const char **value);

/**
 * Returns the attribute of type integer designated by \p id. If the attribute is
 * not available at the specified layer, the attribute is not of type integer,
 * or another error occured, return parameter \p valid will be set accordingly.
 *
 * @see qc_get_attribute_string()
 * @see qc_get_attribute_float()
 *
 * @param hdl Handle of the configuration to use.
 * @param id Attribute to retrieve.
 * @param layer Specifies the layer, e.g.
 * - 0: CEC layer information,
 * - 1: LPAR layer information, etc.
 * @param value Return parameter returning the string attribute's value or undefined
 * in case of an error.
 @return Indicating validity of the queried attribute as follows:
 * - >0  attribute is valid
 * -  0  attribute exists but is not set
 * - <0  an error occurred retrieving the attribute
 * @return
 */
int qc_get_attribute_int(void *hdl, enum qc_attr_id id, int layer, int *value);

/**
 * Returns the attribute of type float designated by \p id. If the attribute is
 * not available at the specified layer, the attribute is not of type float,
 * or another error occured, return parameter \p valid will be set accordingly.
 *
 * @see qc_get_attribute_string()
 * @see qc_get_attribute_int()
 *
 * @param hdl Handle of the configuration to use.
 * @param id Attribute to retrieve.
 * @param layer Specifies the layer, e.g.
 * - 0: CEC layer information,
 * - 1: LPAR layer information, etc.
 * @param value Return parameter returning the float attribute's value or undefined
 * in case of an error.indicating validity as follows:
 @return Indicating validity of the queried attribute as follows:
 * - >0  attribute is valid
 * -  0  attribute exists but is not set
 * - <0  an error occurred retrieving the attribute
 */
int qc_get_attribute_float(void *hdl, enum qc_attr_id id, int layer, float *value);

#endif