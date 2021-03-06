/* Copyright IBM Corp. 2013, 2018 */

//_GNU_SOURCE used for getline and posix_memalign
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/stat.h>
#include "query_capacity_data.h"


/*
 * Below are the structures that define the attributes. The attributes are
 * referenced as an enum, see documentation in query_capacity.h.
 *
 * Note that strings (char pointers) carry the trailing zero byte.
 */

#define QC_LAYER_TYPE_LEN 20
#define QC_LAYER_CAT_LEN   6

struct qc_cec_values {
	int layer_type_num;
	int layer_category_num;
	char layer_type[QC_LAYER_TYPE_LEN];
	char layer_category[QC_LAYER_CAT_LEN];
	char layer_name[9];
	char manufacturer[17];
	char type[5];
	char model_capacity[17];
	char model[17];
	char sequence_code[17];
	char plant[5];
	int num_core_total;
	int num_core_configured;
	int num_core_standby;
	int num_core_reserved;
	int num_core_dedicated;
	int num_core_shared;
	int num_cp_total;
	int num_cp_dedicated;
	int num_cp_shared;
	int num_ifl_total;
	int num_ifl_dedicated;
	int num_ifl_shared;
	int num_cp_threads;
	int num_ifl_threads;
	float capability;
	float secondary_capability;
	int capacity_adjustment_indication;
	int capacity_change_reason;
};

/*
 * valid attributes for a layer, where "layer-type"=="LPAR-Group"
 */
struct qc_lpar_group_values {
	int layer_type_num;
	int layer_category_num;
	char layer_type[QC_LAYER_TYPE_LEN];
	char layer_category[QC_LAYER_CAT_LEN];
	char layer_name[9];
	int cp_absolute_capping;
	int ifl_absolute_capping;
};

/*
 * valid attributes for a layer, where "layer-type"=="LPAR"
 */
struct qc_lpar_values {
	int layer_type_num;
	int layer_category_num;
	char layer_type[QC_LAYER_TYPE_LEN];
	char layer_category[QC_LAYER_CAT_LEN];
	int partition_number;
	char partition_char[26];
	int partition_char_num;
	char layer_name[9];
	char layer_extended_name[257];
	char layer_uuid[37];
	int adjustment;
	int num_core_total;
	int num_core_configured;
	int num_core_standby;
	int num_core_reserved;
	int num_core_dedicated;
	int num_core_shared;
	int num_cp_total;
	int num_cp_dedicated;
	int num_cp_shared;
	int num_ifl_total;
	int num_ifl_dedicated;
	int num_ifl_shared;
	int num_cp_threads;
	int num_ifl_threads;
	int cp_absolute_capping;
	int ifl_absolute_capping;
	int cp_weight_capping;
	int ifl_weight_capping;
};

struct qc_zvm_pool_values {
	int layer_type_num;
	int layer_category_num;
	char layer_type[QC_LAYER_TYPE_LEN];
	char layer_category[QC_LAYER_CAT_LEN];
	char layer_name[9];
	int cp_limithard_cap;
	int cp_capacity_cap;
	int ifl_limithard_cap;
	int ifl_capacity_cap;
	int cp_capped_capacity;
	int ifl_capped_capacity;
};

struct qc_zvm_hypervisor_values {
	int layer_type_num;
	int layer_category_num;
	char layer_type[QC_LAYER_TYPE_LEN];
	char layer_category[QC_LAYER_CAT_LEN];
	char layer_name[9];
	char cluster_name[9];
	char control_program_id[17];
	int adjustment;
	int hardlimit_consumption;
	int prorated_core_time;
	int num_core_total;
	int num_core_dedicated;
	int num_core_shared;
	int num_cp_total;
	int num_cp_dedicated;
	int num_cp_shared;
	int num_ifl_total;
	int num_ifl_dedicated;
	int num_ifl_shared;
	int num_cp_threads;
	int num_ifl_threads;
};

struct qc_zvm_guest_values {
	int layer_type_num;
	int layer_category_num;
	char layer_type[QC_LAYER_TYPE_LEN];
	char layer_category[QC_LAYER_CAT_LEN];
	char layer_name[9];
	char system_identifier[9];
	char cluster_name[9];
	char control_program_id[17];
	int adjustment;
	char capping[5];
	int capping_num;
	int num_cpu_total;
	int num_cpu_configured;
	int num_cpu_standby;
	int num_cpu_reserved;
	int num_cpu_dedicated;
	int num_cpu_shared;
	int num_cp_total;
	int num_cp_dedicated;
	int num_cp_shared;
	int num_ifl_total;
	int num_ifl_dedicated;
	int num_ifl_shared;
	int mobility_enabled;
	int has_multiple_cpu_types;
	int cp_dispatch_limithard;
	int cp_dispatch_type;
	int cp_capped_capacity;
	int ifl_dispatch_limithard;
	int ifl_dispatch_type;
	int ifl_capped_capacity;
};

struct qc_kvm_hypervisor_values {
	int layer_type_num;
	int layer_category_num;
	char layer_type[QC_LAYER_TYPE_LEN];
	char layer_category[QC_LAYER_CAT_LEN];
	char control_program_id[17];
	int adjustment;
	int num_core_total;
	int num_core_dedicated;
	int num_core_shared;
	int num_cp_total;
	int num_cp_dedicated;
	int num_cp_shared;
	int num_ifl_total;
	int num_ifl_dedicated;
	int num_ifl_shared;
};

struct qc_kvm_guest_values {
	int layer_type_num;
	int layer_category_num;
	char layer_type[QC_LAYER_TYPE_LEN];
	char layer_category[QC_LAYER_CAT_LEN];
	char layer_name[9];
	char layer_extended_name[257];
	char layer_uuid[37];
	int num_cpu_total;
	int num_cpu_configured;
	int num_cpu_standby;
	int num_cpu_reserved;
	int num_cpu_dedicated;
	int num_cpu_shared;
	int num_ifl_total;
	int num_ifl_dedicated;
	int num_ifl_shared;
	int ifl_dispatch_type;
};

enum qc_data_type {
	string,
	integer,
	floatingpoint
};

struct qc_attr {
	enum qc_attr_id id;
	enum qc_data_type type;
	int offset;
};

static struct qc_attr cec_attrs[]  =  {
	{qc_layer_type_num, integer, offsetof(struct qc_cec_values, layer_type_num)},
	{qc_layer_category_num, integer, offsetof(struct qc_cec_values, layer_category_num)},
	{qc_layer_type, string, offsetof(struct qc_cec_values, layer_type)},
	{qc_layer_category, string, offsetof(struct qc_cec_values, layer_category)},
	{qc_layer_name, string, offsetof(struct qc_cec_values, layer_name)},
	{qc_manufacturer, string, offsetof(struct qc_cec_values, manufacturer)},
	{qc_type, string, offsetof(struct qc_cec_values, type)},
	{qc_model_capacity, string, offsetof(struct qc_cec_values, model_capacity)},
	{qc_model, string, offsetof(struct qc_cec_values, model)},
	{qc_sequence_code, string, offsetof(struct qc_cec_values, sequence_code)},
	{qc_plant, string, offsetof(struct qc_cec_values, plant)},
	{qc_num_core_total, integer, offsetof(struct qc_cec_values, num_core_total)},
	{qc_num_core_configured, integer, offsetof(struct qc_cec_values, num_core_configured)},
	{qc_num_core_standby, integer, offsetof(struct qc_cec_values, num_core_standby)},
	{qc_num_core_reserved, integer, offsetof(struct qc_cec_values, num_core_reserved)},
	{qc_num_core_dedicated, integer, offsetof(struct qc_cec_values, num_core_dedicated)},
	{qc_num_core_shared, integer, offsetof(struct qc_cec_values, num_core_shared)},
	{qc_num_cp_total, integer, offsetof(struct qc_cec_values, num_cp_total)},
	{qc_num_cp_dedicated, integer, offsetof(struct qc_cec_values, num_cp_dedicated)},
	{qc_num_cp_shared, integer, offsetof(struct qc_cec_values, num_cp_shared)},
	{qc_num_ifl_total, integer, offsetof(struct qc_cec_values, num_ifl_total)},
	{qc_num_ifl_dedicated, integer, offsetof(struct qc_cec_values, num_ifl_dedicated)},
	{qc_num_ifl_shared, integer, offsetof(struct qc_cec_values, num_ifl_shared)},
	{qc_num_cp_threads, integer, offsetof(struct qc_cec_values, num_cp_threads)},
	{qc_num_ifl_threads, integer, offsetof(struct qc_cec_values, num_ifl_threads)},
	{qc_capability, floatingpoint, offsetof(struct qc_cec_values, capability)},
	{qc_secondary_capability, floatingpoint, offsetof(struct qc_cec_values, secondary_capability)},
	{qc_capacity_adjustment_indication, integer, offsetof(struct qc_cec_values, capacity_adjustment_indication)},
	{qc_capacity_change_reason, integer, offsetof(struct qc_cec_values, capacity_change_reason)},
	{-1, string, -1}
};

static struct qc_attr lpar_group_attrs[] = {
	{qc_layer_type_num, integer, offsetof(struct qc_lpar_group_values, layer_type_num)},
	{qc_layer_category_num, integer, offsetof(struct qc_lpar_group_values, layer_category_num)},
	{qc_layer_type, string, offsetof(struct qc_lpar_group_values, layer_type)},
	{qc_layer_category, string, offsetof(struct qc_lpar_group_values, layer_category)},
	{qc_layer_name, string, offsetof(struct qc_lpar_group_values, layer_name)},
	{qc_cp_absolute_capping, integer, offsetof(struct qc_lpar_group_values, cp_absolute_capping)},
	{qc_ifl_absolute_capping, integer, offsetof(struct qc_lpar_group_values, ifl_absolute_capping)},
	{-1, string, -1}
};

static struct qc_attr lpar_attrs[] = {
	{qc_layer_type_num, integer, offsetof(struct qc_lpar_values, layer_type_num)},
	{qc_layer_category_num, integer, offsetof(struct qc_lpar_values, layer_category_num)},
	{qc_layer_type, string, offsetof(struct qc_lpar_values, layer_type)},
	{qc_layer_category, string, offsetof(struct qc_lpar_values, layer_category)},
	{qc_partition_number, integer, offsetof(struct qc_lpar_values, partition_number)},
	{qc_partition_char, string, offsetof(struct qc_lpar_values, partition_char)},
	{qc_partition_char_num, integer, offsetof(struct qc_lpar_values, partition_char_num)},
	{qc_layer_name, string, offsetof(struct qc_lpar_values, layer_name)},
	{qc_layer_extended_name, string, offsetof(struct qc_lpar_values, layer_extended_name)},
	{qc_layer_uuid, string, offsetof(struct qc_lpar_values, layer_uuid)},
	{qc_adjustment, integer, offsetof(struct qc_lpar_values, adjustment)},
	{qc_num_core_total, integer, offsetof(struct qc_lpar_values, num_core_total)},
	{qc_num_core_configured, integer, offsetof(struct qc_lpar_values, num_core_configured)},
	{qc_num_core_standby, integer, offsetof(struct qc_lpar_values, num_core_standby)},
	{qc_num_core_reserved, integer, offsetof(struct qc_lpar_values, num_core_reserved)},
	{qc_num_core_dedicated, integer, offsetof(struct qc_lpar_values, num_core_dedicated)},
	{qc_num_core_shared, integer, offsetof(struct qc_lpar_values, num_core_shared)},
	{qc_num_cp_total, integer, offsetof(struct qc_lpar_values, num_cp_total)},
	{qc_num_cp_dedicated, integer, offsetof(struct qc_lpar_values, num_cp_dedicated)},
	{qc_num_cp_shared, integer, offsetof(struct qc_lpar_values, num_cp_shared)},
	{qc_num_ifl_total, integer, offsetof(struct qc_lpar_values, num_ifl_total)},
	{qc_num_ifl_dedicated, integer, offsetof(struct qc_lpar_values, num_ifl_dedicated)},
	{qc_num_ifl_shared, integer, offsetof(struct qc_lpar_values, num_ifl_shared)},
	{qc_num_cp_threads, integer, offsetof(struct qc_lpar_values, num_cp_threads)},
	{qc_num_ifl_threads, integer, offsetof(struct qc_lpar_values, num_ifl_threads)},
	{qc_cp_absolute_capping, integer, offsetof(struct qc_lpar_values, cp_absolute_capping)},
	{qc_ifl_absolute_capping, integer, offsetof(struct qc_lpar_values, ifl_absolute_capping)},
	{qc_cp_weight_capping, integer, offsetof(struct qc_lpar_values, cp_weight_capping)},
	{qc_ifl_weight_capping, integer, offsetof(struct qc_lpar_values, ifl_weight_capping)},
	{-1, string, -1}
};

static struct qc_attr zvm_hv_attrs[] = {
	{qc_layer_type_num, integer, offsetof(struct qc_zvm_hypervisor_values, layer_type_num)},
	{qc_layer_category_num, integer, offsetof(struct qc_zvm_hypervisor_values, layer_category_num)},
	{qc_layer_type, string, offsetof(struct qc_zvm_hypervisor_values, layer_type)},
	{qc_layer_category, string, offsetof(struct qc_zvm_hypervisor_values, layer_category)},
	{qc_layer_name, string, offsetof(struct qc_zvm_hypervisor_values, layer_name)},
	{qc_cluster_name, string, offsetof(struct qc_zvm_hypervisor_values, cluster_name)},
	{qc_control_program_id, string, offsetof(struct qc_zvm_hypervisor_values, control_program_id)},
	{qc_adjustment, integer, offsetof(struct qc_zvm_hypervisor_values, adjustment)},
	{qc_hardlimit_consumption, integer, offsetof(struct qc_zvm_hypervisor_values, hardlimit_consumption)},
	{qc_prorated_core_time, integer, offsetof(struct qc_zvm_hypervisor_values, prorated_core_time)},
	{qc_num_core_total, integer, offsetof(struct qc_zvm_hypervisor_values, num_core_total)},
	{qc_num_core_dedicated, integer, offsetof(struct qc_zvm_hypervisor_values, num_core_dedicated)},
	{qc_num_core_shared, integer, offsetof(struct qc_zvm_hypervisor_values, num_core_shared)},
	{qc_num_cp_total, integer, offsetof(struct qc_zvm_hypervisor_values, num_cp_total)},
	{qc_num_cp_dedicated, integer, offsetof(struct qc_zvm_hypervisor_values, num_cp_dedicated)},
	{qc_num_cp_shared, integer, offsetof(struct qc_zvm_hypervisor_values, num_cp_shared)},
	{qc_num_ifl_total, integer, offsetof(struct qc_zvm_hypervisor_values, num_ifl_total)},
	{qc_num_ifl_dedicated, integer, offsetof(struct qc_zvm_hypervisor_values, num_ifl_dedicated)},
	{qc_num_ifl_shared, integer, offsetof(struct qc_zvm_hypervisor_values, num_ifl_shared)},
	{qc_num_cp_threads, integer, offsetof(struct qc_zvm_hypervisor_values, num_cp_threads)},
	{qc_num_ifl_threads, integer, offsetof(struct qc_zvm_hypervisor_values, num_ifl_threads)},
	{-1, string, -1}
};

static struct qc_attr kvm_hv_attrs[] = {
	{qc_layer_type_num, integer, offsetof(struct qc_kvm_hypervisor_values, layer_type_num)},
	{qc_layer_category_num, integer, offsetof(struct qc_kvm_hypervisor_values, layer_category_num)},
	{qc_layer_type, string, offsetof(struct qc_kvm_hypervisor_values, layer_type)},
	{qc_layer_category, string, offsetof(struct qc_kvm_hypervisor_values, layer_category)},
	{qc_control_program_id, string, offsetof(struct qc_kvm_hypervisor_values, control_program_id)},
	{qc_adjustment, integer, offsetof(struct qc_kvm_hypervisor_values, adjustment)},
	{qc_num_core_total, integer, offsetof(struct qc_kvm_hypervisor_values, num_core_total)},
	{qc_num_core_dedicated, integer, offsetof(struct qc_kvm_hypervisor_values, num_core_dedicated)},
	{qc_num_core_shared, integer, offsetof(struct qc_kvm_hypervisor_values, num_core_shared)},
	{qc_num_cp_total, integer, offsetof(struct qc_kvm_hypervisor_values, num_cp_total)},
	{qc_num_cp_dedicated, integer, offsetof(struct qc_kvm_hypervisor_values, num_cp_dedicated)},
	{qc_num_cp_shared, integer, offsetof(struct qc_kvm_hypervisor_values, num_cp_shared)},
	{qc_num_ifl_total, integer, offsetof(struct qc_kvm_hypervisor_values, num_ifl_total)},
	{qc_num_ifl_dedicated, integer, offsetof(struct qc_kvm_hypervisor_values, num_ifl_dedicated)},
	{qc_num_ifl_shared, integer, offsetof(struct qc_kvm_hypervisor_values, num_ifl_shared)},
	{-1, string, -1}
};

static struct qc_attr zvm_pool_attrs[] = {
	{qc_layer_type_num, integer, offsetof(struct qc_zvm_pool_values, layer_type_num)},
	{qc_layer_category_num, integer, offsetof(struct qc_zvm_pool_values, layer_category_num)},
	{qc_layer_type, string, offsetof(struct qc_zvm_pool_values, layer_type)},
	{qc_layer_category, string, offsetof(struct qc_zvm_pool_values, layer_category)},
	{qc_layer_name, string, offsetof(struct qc_zvm_pool_values, layer_name)},
	{qc_cp_limithard_cap, integer, offsetof(struct qc_zvm_pool_values, cp_limithard_cap)},
	{qc_cp_capacity_cap, integer, offsetof(struct qc_zvm_pool_values, cp_capacity_cap)},
	{qc_ifl_limithard_cap, integer, offsetof(struct qc_zvm_pool_values, ifl_limithard_cap)},
	{qc_ifl_capacity_cap, integer, offsetof(struct qc_zvm_pool_values, ifl_capacity_cap)},
	{qc_cp_capped_capacity, integer, offsetof(struct qc_zvm_pool_values, cp_capped_capacity)},
	{qc_ifl_capped_capacity, integer, offsetof(struct qc_zvm_pool_values, ifl_capped_capacity)},
	{-1, string, -1}
};

static struct qc_attr zvm_guest_attrs[] = {
	{qc_layer_type_num, integer, offsetof(struct qc_zvm_guest_values, layer_type_num)},
	{qc_layer_category_num, integer, offsetof(struct qc_zvm_guest_values, layer_category_num)},
	{qc_layer_type, string, offsetof(struct qc_zvm_guest_values, layer_type)},
	{qc_layer_category, string, offsetof(struct qc_zvm_guest_values, layer_category)},
	{qc_layer_name, string, offsetof(struct qc_zvm_guest_values, layer_name)},
	{qc_capping, string, offsetof(struct qc_zvm_guest_values, capping)},
	{qc_capping_num, integer, offsetof(struct qc_zvm_guest_values, capping_num)},
	{qc_num_cpu_total, integer, offsetof(struct qc_zvm_guest_values, num_cpu_total)},
	{qc_num_cpu_configured, integer, offsetof(struct qc_zvm_guest_values, num_cpu_configured)},
	{qc_num_cpu_standby, integer, offsetof(struct qc_zvm_guest_values, num_cpu_standby)},
	{qc_num_cpu_reserved, integer, offsetof(struct qc_zvm_guest_values, num_cpu_reserved)},
	{qc_num_cpu_dedicated, integer, offsetof(struct qc_zvm_guest_values, num_cpu_dedicated)},
	{qc_num_cpu_shared, integer, offsetof(struct qc_zvm_guest_values, num_cpu_shared)},
	{qc_num_cp_total, integer, offsetof(struct qc_zvm_guest_values, num_cp_total)},
	{qc_num_cp_dedicated, integer, offsetof(struct qc_zvm_guest_values, num_cp_dedicated)},
	{qc_num_cp_shared, integer, offsetof(struct qc_zvm_guest_values, num_cp_shared)},
	{qc_num_ifl_total, integer, offsetof(struct qc_zvm_guest_values, num_ifl_total)},
	{qc_num_ifl_dedicated, integer, offsetof(struct qc_zvm_guest_values, num_ifl_dedicated)},
	{qc_num_ifl_shared, integer, offsetof(struct qc_zvm_guest_values, num_ifl_shared)},
	{qc_mobility_enabled, integer, offsetof(struct qc_zvm_guest_values, mobility_enabled)},
	{qc_has_multiple_cpu_types, integer, offsetof(struct qc_zvm_guest_values, has_multiple_cpu_types)},
	{qc_cp_dispatch_limithard, integer, offsetof(struct qc_zvm_guest_values, cp_dispatch_limithard)},
	{qc_cp_capped_capacity, integer, offsetof(struct qc_zvm_guest_values, cp_capped_capacity)},
	{qc_ifl_dispatch_limithard, integer, offsetof(struct qc_zvm_guest_values, ifl_dispatch_limithard)},
	{qc_ifl_capped_capacity, integer, offsetof(struct qc_zvm_guest_values, ifl_capped_capacity)},
	{qc_cp_dispatch_type, integer, offsetof(struct qc_zvm_guest_values, cp_dispatch_type)},
	{qc_ifl_dispatch_type, integer, offsetof(struct qc_zvm_guest_values, ifl_dispatch_type)},
	{-1, string, -1}
};

static struct qc_attr kvm_guest_attrs[] = {
	{qc_layer_type_num, integer, offsetof(struct qc_kvm_guest_values, layer_type_num)},
	{qc_layer_category_num, integer, offsetof(struct qc_kvm_guest_values, layer_category_num)},
	{qc_layer_type, string, offsetof(struct qc_kvm_guest_values, layer_type)},
	{qc_layer_category, string, offsetof(struct qc_kvm_guest_values, layer_category)},
	{qc_layer_name, string, offsetof(struct qc_kvm_guest_values, layer_name)},
	{qc_layer_extended_name, string, offsetof(struct qc_kvm_guest_values, layer_extended_name)},
	{qc_layer_uuid, string, offsetof(struct qc_kvm_guest_values, layer_uuid)},
	{qc_num_cpu_total, integer, offsetof(struct qc_kvm_guest_values, num_cpu_total)},
	{qc_num_cpu_configured, integer, offsetof(struct qc_kvm_guest_values, num_cpu_configured)},
	{qc_num_cpu_standby, integer, offsetof(struct qc_kvm_guest_values, num_cpu_standby)},
	{qc_num_cpu_reserved, integer, offsetof(struct qc_kvm_guest_values, num_cpu_reserved)},
	{qc_num_cpu_dedicated, integer, offsetof(struct qc_kvm_guest_values, num_cpu_dedicated)},
	{qc_num_cpu_shared, integer, offsetof(struct qc_kvm_guest_values, num_cpu_shared)},
	{qc_num_ifl_total, integer, offsetof(struct qc_kvm_guest_values, num_ifl_total)},
	{qc_num_ifl_dedicated, integer, offsetof(struct qc_kvm_guest_values, num_ifl_dedicated)},
	{qc_num_ifl_shared, integer, offsetof(struct qc_kvm_guest_values, num_ifl_shared)},
	{qc_ifl_dispatch_type, integer, offsetof(struct qc_kvm_guest_values, ifl_dispatch_type)},
	{-1, string, -1}
};


const char *qc_attr_id_to_char(struct qc_handle *hdl, enum qc_attr_id id) {
	switch (id) {
	case qc_layer_type_num: return "layer_type_num";
	case qc_layer_category_num: return "layer_category_num";
	case qc_layer_type: return "layer_type";
	case qc_layer_category: return "layer_category";
	case qc_layer_name: return "layer_name";
	case qc_layer_extended_name: return "layer_extended_name";
	case qc_layer_uuid: return "layer_uuid";
	case qc_manufacturer: return "manufacturer";
	case qc_type: return "type";
	case qc_model_capacity: return "model_capacity";
	case qc_model: return "model";
	case qc_sequence_code: return "sequence_code";
	case qc_plant: return "plant";
	case qc_num_cpu_total: return "num_cpu_total";
	case qc_num_cpu_configured: return "num_cpu_configured";
	case qc_num_cpu_standby: return "num_cpu_standby";
	case qc_num_cpu_reserved: return "num_cpu_reserved";
	case qc_num_cpu_dedicated: return "num_cpu_dedicated";
	case qc_num_cpu_shared: return "num_cpu_shared";
	case qc_num_cp_total: return "num_cp_total";
	case qc_num_cp_dedicated: return "num_cp_dedicated";
	case qc_num_cp_shared: return "num_cp_shared";
	case qc_num_ifl_total: return "num_ifl_total";
	case qc_num_ifl_dedicated: return "num_ifl_dedicated";
	case qc_num_ifl_shared: return "num_ifl_shared";
	case qc_capability: return "capability";
	case qc_secondary_capability: return "secondary_capability";
	case qc_capacity_adjustment_indication: return "capacity_adjustment_indication";
	case qc_capacity_change_reason: return "capacity_change_reason";
	case qc_partition_number: return "partition_number";
	case qc_partition_char: return "partition_char";
	case qc_partition_char_num: return "partition_char_num";
	case qc_adjustment: return "adjustment";
	case qc_cp_absolute_capping: return "cp_absolute_capping";
	case qc_ifl_absolute_capping: return "ifl_absolute_capping";
	case qc_cp_weight_capping: return "cp_weight_capping";
	case qc_ifl_weight_capping: return "ifl_weight_capping";
	case qc_cluster_name: return "cluster_name";
	case qc_control_program_id: return "control_program_id";
	case qc_hardlimit_consumption: return "hardlimit_consumption";
	case qc_prorated_core_time: return "prorated_core_time";
	case qc_cp_limithard_cap: return "pool_cp_limithard_cap";
	case qc_cp_capacity_cap: return "pool_cp_capacity_cap";
	case qc_ifl_limithard_cap: return "pool_ifl_limithard_cap";
	case qc_ifl_capacity_cap: return "pool_ifl_capacity_cap";
	case qc_capping: return "capping";
	case qc_capping_num: return "capping_num";
	case qc_mobility_enabled: return "mobility_enabled";
	case qc_has_multiple_cpu_types: return "has_multiple_cpu_types";
	case qc_cp_dispatch_limithard: return "cp_dispatch_limithard";
	case qc_ifl_dispatch_limithard: return "ifl_dispatch_limithard";
	case qc_cp_dispatch_type: return "cp_dispatch_type";
	case qc_ifl_dispatch_type: return "ifl_dispatch_type";
	case qc_cp_capped_capacity: return "cp_capped_capacity";
	case qc_ifl_capped_capacity: return "ifl_capped_capacity";
	case qc_num_cp_threads: return "num_cp_threads";
	case qc_num_ifl_threads: return "num_ifl_threads";
	case qc_num_core_total: return "num_core_total";
	case qc_num_core_configured: return "num_core_configured";
	case qc_num_core_standby: return "num_core_standby";
	case qc_num_core_reserved: return "num_core_reserved";
	case qc_num_core_dedicated: return "num_core_dedicated";
	case qc_num_core_shared: return "num_core_shared";
	default: break;
	}
	qc_debug(hdl, "Error: Cannot convert unknown attribute '%d' to char*\n", id);

	return NULL;
}

// 'hdl' is for error reporting, as 'tgthdl' might not be part of the pointer lists yet
int qc_new_handle(struct qc_handle *hdl, struct qc_handle **tgthdl, int layer_no,
		  int layer_type_num) {
	int num_attrs, layer_category_num;
	char *layer_type, *layer_category;
	struct qc_attr *attrs;
	size_t layer_sz;

	switch (layer_type_num) {
	case QC_LAYER_TYPE_CEC:
		layer_sz = sizeof(struct qc_cec_values);
		attrs = cec_attrs;
		layer_category_num = QC_LAYER_CAT_HOST;
		layer_category = "HOST";
		layer_type = "CEC";
		break;
	case QC_LAYER_TYPE_LPAR_GROUP:
		layer_sz = sizeof(struct qc_lpar_values);
		attrs = lpar_group_attrs;
		layer_category_num = QC_LAYER_CAT_POOL;
		layer_category = "POOL";
		layer_type = "LPAR-GROUP";
		break;
	case QC_LAYER_TYPE_LPAR:
		layer_sz = sizeof(struct qc_lpar_values);
		attrs = lpar_attrs;
		layer_category_num = QC_LAYER_CAT_GUEST;
		layer_category = "GUEST";
		layer_type = "LPAR";
		break;
	case QC_LAYER_TYPE_ZVM_HYPERVISOR:
		layer_sz = sizeof(struct qc_zvm_hypervisor_values);
		attrs = zvm_hv_attrs;
		layer_category_num = QC_LAYER_CAT_HOST;
		layer_category = "HOST";
		layer_type = "z/VM-hypervisor";
		break;
	case QC_LAYER_TYPE_ZVM_RESOURCE_POOL:
		layer_sz = sizeof(struct qc_zvm_pool_values);
		attrs = zvm_pool_attrs;
		layer_category_num = QC_LAYER_CAT_POOL;
		layer_category = "POOL";
#ifdef CONFIG_V1_COMPATIBILITY
		layer_type = "z/VM-CPU-pool";
#else
		layer_type = "z/VM-resource-pool";
#endif
		break;
	case QC_LAYER_TYPE_ZVM_GUEST:
		layer_sz = sizeof(struct qc_zvm_guest_values);
		attrs = zvm_guest_attrs;
		layer_category_num = QC_LAYER_CAT_GUEST;
		layer_category = "GUEST";
		layer_type = "z/VM-guest";
		break;
	case QC_LAYER_TYPE_KVM_HYPERVISOR:
		layer_sz = sizeof(struct qc_kvm_hypervisor_values);
		attrs = kvm_hv_attrs;
		layer_category_num = QC_LAYER_CAT_HOST;
		layer_category = "HOST";
		layer_type = "KVM-hypervisor";
		break;
	case QC_LAYER_TYPE_KVM_GUEST:
		layer_sz = sizeof(struct qc_kvm_guest_values);
		attrs = kvm_guest_attrs;
		layer_category_num = QC_LAYER_CAT_GUEST;
		layer_category = "GUEST";
		layer_type =  "KVM-guest";
		break;
	default:
		qc_debug(hdl, "Error: Unhandled layer type in qc_new_handle()\n");
		return -1;
	}

	// determine number of attributes
	for (num_attrs = 0; attrs[num_attrs].offset >= 0; ++num_attrs);
	num_attrs++;

	if (hdl || *tgthdl == NULL) {
		// Possibly reuse existing handle when alloc'ing the cec layer.
		// Otherwise we'd change the handle which serves as an identified in
		// our log output, which could be confusing.
		*tgthdl = malloc(sizeof(struct qc_handle));
		if (!*tgthdl) {
			qc_debug(hdl, "Error: Failed to allocate handle\n");
			return -2;
		}
	}
	memset(*tgthdl, 0, sizeof(struct qc_handle));
	(*tgthdl)->layer_no = layer_no;
	(*tgthdl)->attr_list = attrs;
	if (hdl)
		(*tgthdl)->root = hdl->root;
	else
		(*tgthdl)->root = *tgthdl;
	(*tgthdl)->layer = malloc(layer_sz);
	if (!(*tgthdl)->layer) {
		qc_debug(hdl, "Error: Failed to allocate layer\n");
		free(*tgthdl);
		*tgthdl = NULL;
		return -3;
	}
	memset((*tgthdl)->layer, 0, layer_sz);
	(*tgthdl)->attr_present = calloc(num_attrs, sizeof(int));
	(*tgthdl)->src = calloc(num_attrs, sizeof(int));
	if (!(*tgthdl)->attr_present || !(*tgthdl)->src) {
		qc_debug(hdl, "Error: Failed to allocate attr_present array\n");
		free((*tgthdl)->layer);
		free(*tgthdl);
		*tgthdl = NULL;
		return -4;
	}
	if (qc_set_attr_int(*tgthdl, qc_layer_type_num, layer_type_num, ATTR_SRC_UNDEF) ||
	    qc_set_attr_int(*tgthdl, qc_layer_category_num, layer_category_num, ATTR_SRC_UNDEF) ||
	    qc_set_attr_string(*tgthdl, qc_layer_type, layer_type, strlen(layer_type), ATTR_SRC_UNDEF) ||
	    qc_set_attr_string(*tgthdl, qc_layer_category, layer_category, strlen(layer_category), ATTR_SRC_UNDEF))
		return -5;

	return 0;
}

int qc_insert_handle(struct qc_handle *hdl, struct qc_handle **inserted_hdl, int type) {
	struct qc_handle *prev_hdl = qc_get_prev_handle(hdl);

	if (!prev_hdl)
		return -1;
	if (qc_new_handle(hdl, inserted_hdl, hdl->layer_no, type))
		return -2;
	(*inserted_hdl)->next = hdl;
	prev_hdl->next = *inserted_hdl;
	// adjust layer_no in remaining layers
	for (; hdl != NULL; hdl = hdl->next)
		hdl->layer_no++;

	return 0;
}

int qc_append_handle(struct qc_handle *hdl, struct qc_handle **appended_hdl, int type) {
	struct qc_handle *next_hdl = hdl->next;

	if (qc_new_handle(hdl, appended_hdl, hdl->layer_no + 1, type))
		return -1;
	hdl->next = *appended_hdl;
	(*appended_hdl)->next = next_hdl;
	// adjust layer_no in remaining layers
	for (hdl = next_hdl; hdl != NULL; hdl = hdl->next)
		hdl->layer_no++;

	return 0;
}

#ifdef CONFIG_V1_COMPATIBILITY
/* Maps qc_num_cpu_* to qc_num_core_* attributes where required to preserve backwards compatibility.
 * Should be removed in a qclib v2.0 release. */
static enum qc_attr_id preserve_v1_attr_compatibility(struct qc_handle *hdl, enum qc_attr_id id) {
	if (id != qc_layer_type_num) {
		int *layer_type = qc_get_attr_value_int(hdl, qc_layer_type_num);
		switch (*layer_type) {
		case QC_LAYER_TYPE_CEC:
		case QC_LAYER_TYPE_LPAR:
			switch (id) {
			case qc_num_cpu_configured: return qc_num_core_configured;
			case qc_num_cpu_standby: return qc_num_core_standby;
			case qc_num_cpu_reserved: return qc_num_core_reserved;
			default: break;
			}
			// fallthrough
		case QC_LAYER_TYPE_ZVM_HYPERVISOR:
		case QC_LAYER_TYPE_KVM_HYPERVISOR:
			switch (id) {
			case qc_num_cpu_total: return qc_num_core_total;
			case qc_num_cpu_dedicated: return qc_num_core_dedicated;
			case qc_num_cpu_shared: return qc_num_core_shared;
			default: break;
			}
		}
	}

	return id;
}
#endif

// Indicates the attribute as 'set', returning a ptr to its content
static char *qc_set_attr(struct qc_handle *hdl, enum qc_attr_id id, enum qc_data_type type, char src, int *prev_set) {
	struct qc_attr *attr_list = hdl->attr_list;
	int count;

	for (count = 0; attr_list[count].offset >= 0; ++count) {
		if (attr_list[count].id == id && attr_list[count].type == type) {
			*prev_set = hdl->attr_present[count];
			hdl->attr_present[count] = 1;
			hdl->src[count] = src;
			return (char *)hdl->layer + attr_list[count].offset;
		}
	}
	qc_debug(hdl, "Error: Failed to set attr=%s (not found)\n", qc_attr_id_to_char(hdl, id));

	return NULL;
}

// Sets attribute 'id' in layer as pointed to by 'hdl'
int qc_set_attr_int(struct qc_handle *hdl, enum qc_attr_id id, int val, char src) {
	char orig_src = qc_get_attr_value_src_int(hdl, id);
	int *ptr, prev_set;

#ifdef CONFIG_V1_COMPATIBILITY
	id = preserve_v1_attr_compatibility(hdl, id);
#endif
	if ((ptr = (int *)qc_set_attr(hdl, id, integer, src, &prev_set)) == NULL)
		return -1;
	if (qc_consistency_check_requested && prev_set && *ptr != val) {
#ifdef CONFIG_TEXTUAL_HYPFS
		int *layer_type = qc_get_attr_value_int(hdl, qc_layer_type_num);
		// Affects ids qc_num_cp_total and qc_num_ifl_total only
		if (orig_src != ATTR_SRC_HYPFS || *layer_type != QC_LAYER_TYPE_LPAR) {
#endif
			qc_debug(hdl, "Error: Consistency at layer %d: Attr %s had value %d from %c, try to set to %d from %c\n",
				 hdl->layer_no, qc_attr_id_to_char(hdl, id), *ptr, orig_src, val, src);
				return -2;
#ifdef CONFIG_TEXTUAL_HYPFS
		}
#endif
	}
	*ptr = val;

	return 0;
}

// Sets attribute 'id' in layer as pointed to by 'hdl'
int qc_set_attr_float(struct qc_handle *hdl, enum qc_attr_id id, float val, char src) {
	char orig_src = qc_get_attr_value_src_int(hdl, id);
	int prev_set;
	float *ptr;

	if ((ptr = (float *)qc_set_attr(hdl, id, floatingpoint, src, &prev_set)) == NULL)
		return -1;
	if (qc_consistency_check_requested && prev_set && *ptr != val) {
		qc_debug(hdl, "Error: Consistency at layer %d: Attr %s had value %f from %c, try to set to %f from %c\n",
			 hdl->layer_no, qc_attr_id_to_char(hdl, id), *ptr, orig_src, val, src);
		return -2;
	}
	*ptr = val;

	return 0;
}

// Sets string attribute 'id' in layer as pointed to by 'hdl', stripping trailing blanks, but
// leaving the original string unmodified
int qc_set_attr_string(struct qc_handle *hdl, enum qc_attr_id id, const char *str, unsigned int str_len, char src) {
	char orig_src = qc_get_attr_value_src_int(hdl, id);
	char *ptr, *tmp, *s;
	int prev_set;

	if ((ptr = qc_set_attr(hdl, id, string, src, &prev_set)) == NULL)
		return -1;
	if (qc_consistency_check_requested && prev_set) {
		if ((tmp = strdup(str)) == NULL) {
			qc_debug(hdl, "Error: Failed to duplicate string\n");
			return -2;
		}
		for (s = &tmp[strlen(tmp) - 1]; (*s == ' ' || *s == '\n') && s != tmp; --s)
			*s = '\0';
		if (strcmp(ptr, tmp)) {
			qc_debug(hdl, "Error: Consistency at layer %d: Attr %s had value %s from %c, try to set to %s from %c\n",
				hdl->layer_no, qc_attr_id_to_char(hdl, id), ptr, orig_src, tmp, src);
			free(tmp);
			return -3;
		}
		free(tmp);
	}
	ptr[str_len] = '\0';
	strncpy(ptr, str, str_len);
	// strip trailing blanks
	for (s = &ptr[strlen(ptr) - 1]; (*s == ' ' || *s == '\n') && s != ptr; --s)
		*s = '\0';

	return 0;
}

// Sets ebcdic string attribute 'id' in layer as pointed to by 'hdl'
// Note: Copy content to temporary buffer for conversion first, as we do not want to modify the source data.
int qc_set_attr_ebcdic_string(struct qc_handle *hdl, enum qc_attr_id id, unsigned char *str,
			      unsigned int str_len, char src) {
	char *buf;
	int rc;

	buf = malloc(str_len + 1);
	if (!buf) {
		qc_debug(hdl, "Error: Memory allocation error\n");
		return -1;
	}
	memset(buf, '\0', str_len + 1);
	memcpy(buf, str, str_len);
	if ((rc = qc_ebcdic_to_ascii(hdl, buf, str_len)) == 0) {
		if (strlen(buf) && qc_set_attr_string(hdl, id, (char *)buf, str_len, src))
			rc = -2;
	}
	free(buf);

	return rc;
}

int qc_is_nonempty_ebcdic(__u64 *str) {
	// CPU Pools in STHYI have all EBCDIC spaces if not set
	return *str != 0x0 && *str != 0x4040404040404040ULL;
}

// Returns whether attribute 'id' in layer as pointed to by 'hdl' is set/defined
static int qc_is_attr_set(struct qc_handle *hdl, enum qc_attr_id id, enum qc_data_type type) {
	struct qc_attr *attr_list = hdl->attr_list;
	int count = 0;

	while (attr_list[count].offset >= 0) {
		if (attr_list[count].id == id && attr_list[count].type == type)
			return hdl->attr_present[count];
		count++;
	}

	return 0;
}

int qc_is_attr_set_int(struct qc_handle *hdl, enum qc_attr_id id) {
	return qc_is_attr_set(hdl, id, integer);
}

int qc_is_attr_set_float(struct qc_handle *hdl, enum qc_attr_id id) {
	return qc_is_attr_set(hdl, id, floatingpoint);
}

int qc_is_attr_set_string(struct qc_handle *hdl, enum qc_attr_id id) {
	return qc_is_attr_set(hdl, id, string);
}

struct qc_handle *qc_get_root_handle(struct qc_handle *hdl) {
	return hdl ? hdl->root : NULL;
}

struct qc_handle *qc_get_prev_handle(struct qc_handle *hdl) {
	struct qc_handle *prev_hdl = NULL;

	for (prev_hdl = hdl->root; prev_hdl->next != NULL; prev_hdl = prev_hdl->next)
		if (prev_hdl->next == hdl)
			return prev_hdl;
	qc_debug(hdl, "Error: Couldn't find handle pointing at layer %d handle\n", hdl->layer_no);

	return NULL;
}

static int qc_get_attr_idx(struct qc_handle *hdl, enum qc_attr_id id, enum qc_data_type type) {
	struct qc_attr *attr_list = hdl->attr_list;
	int idx;

	for (idx = 0; attr_list[idx].offset >= 0; ++idx)
		if (attr_list[idx].id == id && attr_list[idx].type == type)
			return idx;

	return -1;
}

/// Retrieve value of attribute 'id' of layer pointed at by 'hdl'
static void *qc_get_attr_value(struct qc_handle *hdl, enum qc_attr_id id, enum qc_data_type type) {
	struct qc_attr *attr_list = hdl->attr_list;
	int idx;

	if ((idx = qc_get_attr_idx(hdl, id, type)) < 0 || !hdl->attr_present[idx])
		return NULL;

	return (char *)hdl->layer + attr_list[idx].offset;
}

int *qc_get_attr_value_int(struct qc_handle *hdl, enum qc_attr_id id) {
#ifdef CONFIG_V1_COMPATIBILITY
	id = preserve_v1_attr_compatibility(hdl, id);
#endif
	return (int *)qc_get_attr_value(hdl, id, integer);
}

float *qc_get_attr_value_float(struct qc_handle *hdl, enum qc_attr_id id) {
	return (float *)qc_get_attr_value(hdl, id, floatingpoint);
}

char *qc_get_attr_value_string(struct qc_handle *hdl, enum qc_attr_id id) {
	return (char *)qc_get_attr_value(hdl, id, string);
}

static char qc_get_attr_value_src(struct qc_handle *hdl, enum qc_attr_id id, enum qc_data_type type) {
	int idx;

	if ((idx = qc_get_attr_idx(hdl, id, type)) < 0)
		return 'x';

	return hdl->src[idx];
}

char qc_get_attr_value_src_int(struct qc_handle *hdl, enum qc_attr_id id) {
#ifdef CONFIG_V1_COMPATIBILITY
	id = preserve_v1_attr_compatibility(hdl, id);
#endif
	return qc_get_attr_value_src(hdl, id, integer);
}

char qc_get_attr_value_src_float(struct qc_handle *hdl, enum qc_attr_id id) {
	return qc_get_attr_value_src(hdl, id, floatingpoint);
}

char qc_get_attr_value_src_string(struct qc_handle *hdl, enum qc_attr_id id) {
	return qc_get_attr_value_src(hdl, id, string);
}
