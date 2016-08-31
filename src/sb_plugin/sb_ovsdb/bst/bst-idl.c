/* Generated automatically -- do not modify!    -*- buffer-read-only: t -*- */

#include "bst-idl.h"
#include <limits.h>
#include "ovs-thread.h"
#include "ovsdb-data.h"
#include "ovsdb-error.h"
#include "util.h"

#ifdef __CHECKER__
/* Sparse dislikes sizeof(bool) ("warning: expression using sizeof bool"). */
enum { sizeof_bool = 1 };
#else
enum { sizeof_bool = sizeof(bool) };
#endif

static bool inited;


static struct bst_system *
bst_system_cast(const struct ovsdb_idl_row *row)
{
    return row ? CONTAINER_OF(row, struct bst_system, header_) : NULL;
}

static struct bst_bufmon *
bst_bufmon_cast(const struct ovsdb_idl_row *row)
{
    return row ? CONTAINER_OF(row, struct bst_bufmon, header_) : NULL;
}

/* System table. */

static void
bst_system_parse_bufmon_config(struct ovsdb_idl_row *row_, const struct ovsdb_datum *datum)
{
    struct bst_system *row = bst_system_cast(row_);
    size_t i;

    ovs_assert(inited);
    smap_init(&row->bufmon_config);
    for (i = 0; i < datum->n; i++) {
        smap_add(&row->bufmon_config,
                 datum->keys[i].string,
                 datum->values[i].string);
    }
}

static void
bst_system_parse_bufmon_info(struct ovsdb_idl_row *row_, const struct ovsdb_datum *datum)
{
    struct bst_system *row = bst_system_cast(row_);
    size_t i;

    ovs_assert(inited);
    smap_init(&row->bufmon_info);
    for (i = 0; i < datum->n; i++) {
        smap_add(&row->bufmon_info,
                 datum->keys[i].string,
                 datum->values[i].string);
    }
}

static void
bst_system_unparse_bufmon_config(struct ovsdb_idl_row *row_)
{
    struct bst_system *row = bst_system_cast(row_);

    ovs_assert(inited);
    smap_destroy(&row->bufmon_config);
}

static void
bst_system_unparse_bufmon_info(struct ovsdb_idl_row *row_)
{
    struct bst_system *row = bst_system_cast(row_);

    ovs_assert(inited);
    smap_destroy(&row->bufmon_info);
}

static void
bst_system_init__(struct ovsdb_idl_row *row)
{
    bst_system_init(bst_system_cast(row));
}

/* Clears the contents of 'row' in table "System". */
void
bst_system_init(struct bst_system *row)
{
    memset(row, 0, sizeof *row); 
    smap_init(&row->bufmon_config);
    smap_init(&row->bufmon_info);
}

/* Searches table "System" in 'idl' for a row with UUID 'uuid'.  Returns
 * a pointer to the row if there is one, otherwise a null pointer.  */
const struct bst_system *
bst_system_get_for_uuid(const struct ovsdb_idl *idl, const struct uuid *uuid)
{
    return bst_system_cast(ovsdb_idl_get_row_for_uuid(idl, &bst_table_classes[BST_TABLE_SYSTEM], uuid));
}

/* Returns a row in table "System" in 'idl', or a null pointer if that
 * table is empty.
 *
 * Database tables are internally maintained as hash tables, so adding or
 * removing rows while traversing the same table can cause some rows to be
 * visited twice or not at apply. */
const struct bst_system *
bst_system_first(const struct ovsdb_idl *idl)
{
    return bst_system_cast(ovsdb_idl_first_row(idl, &bst_table_classes[BST_TABLE_SYSTEM]));
}

/* Returns a row following 'row' within its table, or a null pointer if 'row'
 * is the last row in its table. */
const struct bst_system *
bst_system_next(const struct bst_system *row)
{
    return bst_system_cast(ovsdb_idl_next_row(&row->header_));
}

/* Deletes 'row' from table "System".  'row' may be freed, so it must not be
 * accessed afterward.
 *
 * The caller must have started a transaction with ovsdb_idl_txn_create(). */
void
bst_system_delete(const struct bst_system *row)
{
    ovsdb_idl_txn_delete(&row->header_);
}

/* Inserts and returns a new row in the table "System" in the database
 * with open transaction 'txn'.
 *
 * The new row is assigned a randomly generated provisional UUID.
 * ovsdb-server will assign a different UUID when 'txn' is committed,
 * but the IDL will replace any uses of the provisional UUID in the
 * data to be to be committed by the UUID assigned by ovsdb-server. */
struct bst_system *
bst_system_insert(struct ovsdb_idl_txn *txn)
{
    return bst_system_cast(ovsdb_idl_txn_insert(txn, &bst_table_classes[BST_TABLE_SYSTEM], NULL));
}

/* Causes the original contents of column "bufmon_config" in 'row' to be
 * verified as a prerequisite to completing the transaction.  That is, if
 * "bufmon_config" in 'row' changed (or if 'row' was deleted) between the
 * time that the IDL originally read its contents and the time that the
 * transaction commits, then the transaction aborts and ovsdb_idl_txn_commit()
 * returns TXN_AGAIN_WAIT or TXN_AGAIN_NOW (depending on whether the database
 * change has already been received).
 *
 * The intention is that, to ensure that no transaction commits based on dirty
 * reads, an application should call this function any time "bufmon_config" is
 * read as part of a read-modify-write operation.
 *
 * In some cases this function reduces to a no-op, because the current value
 * of "bufmon_config" is already known:
 *
 *   - If 'row' is a row created by the current transaction (returned by
 *     bst_system_insert()).
 *
 *   - If "bufmon_config" has already been modified (with
 *     bst_system_set_bufmon_config()) within the current transaction.
 *
 * Because of the latter property, always call this function *before*
 * bst_system_set_bufmon_config() for a given read-modify-write.
 *
 * The caller must have started a transaction with ovsdb_idl_txn_create(). */
void
bst_system_verify_bufmon_config(const struct bst_system *row)
{
    ovs_assert(inited);
    ovsdb_idl_txn_verify(&row->header_, &bst_system_columns[BST_SYSTEM_COL_BUFMON_CONFIG]);
}

/* Causes the original contents of column "bufmon_info" in 'row' to be
 * verified as a prerequisite to completing the transaction.  That is, if
 * "bufmon_info" in 'row' changed (or if 'row' was deleted) between the
 * time that the IDL originally read its contents and the time that the
 * transaction commits, then the transaction aborts and ovsdb_idl_txn_commit()
 * returns TXN_AGAIN_WAIT or TXN_AGAIN_NOW (depending on whether the database
 * change has already been received).
 *
 * The intention is that, to ensure that no transaction commits based on dirty
 * reads, an application should call this function any time "bufmon_info" is
 * read as part of a read-modify-write operation.
 *
 * In some cases this function reduces to a no-op, because the current value
 * of "bufmon_info" is already known:
 *
 *   - If 'row' is a row created by the current transaction (returned by
 *     bst_system_insert()).
 *
 *   - If "bufmon_info" has already been modified (with
 *     bst_system_set_bufmon_info()) within the current transaction.
 *
 * Because of the latter property, always call this function *before*
 * bst_system_set_bufmon_info() for a given read-modify-write.
 *
 * The caller must have started a transaction with ovsdb_idl_txn_create(). */
void
bst_system_verify_bufmon_info(const struct bst_system *row)
{
    ovs_assert(inited);
    ovsdb_idl_txn_verify(&row->header_, &bst_system_columns[BST_SYSTEM_COL_BUFMON_INFO]);
}

/* Returns the "bufmon_config" column's value from the "System" table in 'row'
 * as a struct ovsdb_datum.  This is useful occasionally: for example,
 * ovsdb_datum_find_key() is an easier and more efficient way to search
 * for a given key than implementing the same operation on the "cooked"
 * form in 'row'.
 *
 * 'key_type' must be OVSDB_TYPE_STRING.
 * 'value_type' must be OVSDB_TYPE_STRING.
 * (This helps to avoid silent bugs if someone changes bufmon_config's
 * type without updating the caller.)
 *
 * The caller must not modify or free the returned value.
 *
 * Various kinds of changes can invalidate the returned value: modifying
 * 'column' within 'row', deleting 'row', or completing an ongoing transaction.
 * If the returned value is needed for a long time, it is best to make a copy
 * of it with ovsdb_datum_clone().
 *
 * This function is rarely useful, since it is easier to access the value
 * directly through the "bufmon_config" member in bst_system. */
const struct ovsdb_datum *
bst_system_get_bufmon_config(const struct bst_system *row,
	enum ovsdb_atomic_type key_type OVS_UNUSED,
	enum ovsdb_atomic_type value_type OVS_UNUSED)
{
    ovs_assert(key_type == OVSDB_TYPE_STRING);
    ovs_assert(value_type == OVSDB_TYPE_STRING);
    return ovsdb_idl_read(&row->header_, &bst_system_col_bufmon_config);
}

/* Returns the "bufmon_info" column's value from the "System" table in 'row'
 * as a struct ovsdb_datum.  This is useful occasionally: for example,
 * ovsdb_datum_find_key() is an easier and more efficient way to search
 * for a given key than implementing the same operation on the "cooked"
 * form in 'row'.
 *
 * 'key_type' must be OVSDB_TYPE_STRING.
 * 'value_type' must be OVSDB_TYPE_STRING.
 * (This helps to avoid silent bugs if someone changes bufmon_info's
 * type without updating the caller.)
 *
 * The caller must not modify or free the returned value.
 *
 * Various kinds of changes can invalidate the returned value: modifying
 * 'column' within 'row', deleting 'row', or completing an ongoing transaction.
 * If the returned value is needed for a long time, it is best to make a copy
 * of it with ovsdb_datum_clone().
 *
 * This function is rarely useful, since it is easier to access the value
 * directly through the "bufmon_info" member in bst_system. */
const struct ovsdb_datum *
bst_system_get_bufmon_info(const struct bst_system *row,
	enum ovsdb_atomic_type key_type OVS_UNUSED,
	enum ovsdb_atomic_type value_type OVS_UNUSED)
{
    ovs_assert(key_type == OVSDB_TYPE_STRING);
    ovs_assert(value_type == OVSDB_TYPE_STRING);
    return ovsdb_idl_read(&row->header_, &bst_system_col_bufmon_info);
}

/* Sets the "bufmon_config" column's value from the "System" table in 'row'
 * to 'bufmon_config'.
 *
 * The caller retains ownership of 'bufmon_config' and everything in it. */
void
bst_system_set_bufmon_config(const struct bst_system *row, const struct smap *bufmon_config)
{
    struct ovsdb_datum datum;

    ovs_assert(inited);
    if (bufmon_config) {
        struct smap_node *node;
        size_t i;

        datum.n = smap_count(bufmon_config);
        datum.keys = xmalloc(datum.n * sizeof *datum.keys);
        datum.values = xmalloc(datum.n * sizeof *datum.values);

        i = 0;
        SMAP_FOR_EACH (node, bufmon_config) {
            datum.keys[i].string = xstrdup(node->key);
            datum.values[i].string = xstrdup(node->value);
            i++;
        }
        ovsdb_datum_sort_unique(&datum, OVSDB_TYPE_STRING, OVSDB_TYPE_STRING);
    } else {
        ovsdb_datum_init_empty(&datum);
    }
    ovsdb_idl_txn_write(&row->header_,
                        &bst_system_columns[BST_SYSTEM_COL_BUFMON_CONFIG],
                        &datum);
}


/* Sets the "bufmon_info" column's value from the "System" table in 'row'
 * to 'bufmon_info'.
 *
 * The caller retains ownership of 'bufmon_info' and everything in it. */
void
bst_system_set_bufmon_info(const struct bst_system *row, const struct smap *bufmon_info)
{
    struct ovsdb_datum datum;

    ovs_assert(inited);
    if (bufmon_info) {
        struct smap_node *node;
        size_t i;

        datum.n = smap_count(bufmon_info);
        datum.keys = xmalloc(datum.n * sizeof *datum.keys);
        datum.values = xmalloc(datum.n * sizeof *datum.values);

        i = 0;
        SMAP_FOR_EACH (node, bufmon_info) {
            datum.keys[i].string = xstrdup(node->key);
            datum.values[i].string = xstrdup(node->value);
            i++;
        }
        ovsdb_datum_sort_unique(&datum, OVSDB_TYPE_STRING, OVSDB_TYPE_STRING);
    } else {
        ovsdb_datum_init_empty(&datum);
    }
    ovsdb_idl_txn_write(&row->header_,
                        &bst_system_columns[BST_SYSTEM_COL_BUFMON_INFO],
                        &datum);
}


struct ovsdb_idl_column bst_system_columns[BST_SYSTEM_N_COLUMNS];

static void
bst_system_columns_init(void)
{
    struct ovsdb_idl_column *c;

    /* Initialize bst_system_col_bufmon_config. */
    c = &bst_system_col_bufmon_config;
    c->name = "bufmon_config";
    ovsdb_base_type_init(&c->type.key, OVSDB_TYPE_STRING);
    c->type.key.u.string.minLen = 0;
    ovsdb_base_type_init(&c->type.value, OVSDB_TYPE_STRING);
    c->type.value.u.string.minLen = 0;
    c->type.n_min = 0;
    c->type.n_max = UINT_MAX;
    c->mutable = true;
    c->parse = bst_system_parse_bufmon_config;
    c->unparse = bst_system_unparse_bufmon_config;

    /* Initialize bst_system_col_bufmon_info. */
    c = &bst_system_col_bufmon_info;
    c->name = "bufmon_info";
    ovsdb_base_type_init(&c->type.key, OVSDB_TYPE_STRING);
    c->type.key.u.string.minLen = 0;
    ovsdb_base_type_init(&c->type.value, OVSDB_TYPE_STRING);
    c->type.value.u.string.minLen = 0;
    c->type.n_min = 0;
    c->type.n_max = UINT_MAX;
    c->mutable = true;
    c->parse = bst_system_parse_bufmon_info;
    c->unparse = bst_system_unparse_bufmon_info;
}

/* bufmon table. */

static void
bst_bufmon_parse_counter_value(struct ovsdb_idl_row *row_, const struct ovsdb_datum *datum)
{
    struct bst_bufmon *row = bst_bufmon_cast(row_);
    size_t n = MIN(1, datum->n);
    size_t i;

    ovs_assert(inited);
    row->counter_value = NULL;
    row->n_counter_value = 0;
    for (i = 0; i < n; i++) {
        if (!row->n_counter_value) {
            row->counter_value = xmalloc(n * sizeof *row->counter_value);
        }
        row->counter_value[row->n_counter_value] = datum->keys[i].integer;
        row->n_counter_value++;
    }
}

static void
bst_bufmon_parse_counter_vendor_specific_info(struct ovsdb_idl_row *row_, const struct ovsdb_datum *datum)
{
    struct bst_bufmon *row = bst_bufmon_cast(row_);
    size_t i;

    ovs_assert(inited);
    smap_init(&row->counter_vendor_specific_info);
    for (i = 0; i < datum->n; i++) {
        smap_add(&row->counter_vendor_specific_info,
                 datum->keys[i].string,
                 datum->values[i].string);
    }
}

static void
bst_bufmon_parse_enabled(struct ovsdb_idl_row *row_, const struct ovsdb_datum *datum)
{
    struct bst_bufmon *row = bst_bufmon_cast(row_);

    ovs_assert(inited);
    if (datum->n >= 1) {
        row->enabled = datum->keys[0].boolean;
    } else {
        row->enabled = false;
    }
}

static void
bst_bufmon_parse_hw_unit_id(struct ovsdb_idl_row *row_, const struct ovsdb_datum *datum)
{
    struct bst_bufmon *row = bst_bufmon_cast(row_);

    ovs_assert(inited);
    if (datum->n >= 1) {
        row->hw_unit_id = datum->keys[0].integer;
    } else {
        row->hw_unit_id = 0;
    }
}

static void
bst_bufmon_parse_name(struct ovsdb_idl_row *row_, const struct ovsdb_datum *datum)
{
    struct bst_bufmon *row = bst_bufmon_cast(row_);

    ovs_assert(inited);
    if (datum->n >= 1) {
        row->name = datum->keys[0].string;
    } else {
        row->name = "";
    }
}

static void
bst_bufmon_parse_status(struct ovsdb_idl_row *row_, const struct ovsdb_datum *datum)
{
    struct bst_bufmon *row = bst_bufmon_cast(row_);

    ovs_assert(inited);
    if (datum->n >= 1) {
        row->status = datum->keys[0].string;
    } else {
        row->status = NULL;
    }
}

static void
bst_bufmon_parse_trigger_threshold(struct ovsdb_idl_row *row_, const struct ovsdb_datum *datum)
{
    struct bst_bufmon *row = bst_bufmon_cast(row_);
    size_t n = MIN(1, datum->n);
    size_t i;

    ovs_assert(inited);
    row->trigger_threshold = NULL;
    row->n_trigger_threshold = 0;
    for (i = 0; i < n; i++) {
        if (!row->n_trigger_threshold) {
            row->trigger_threshold = xmalloc(n * sizeof *row->trigger_threshold);
        }
        row->trigger_threshold[row->n_trigger_threshold] = datum->keys[i].integer;
        row->n_trigger_threshold++;
    }
}

static void
bst_bufmon_unparse_counter_value(struct ovsdb_idl_row *row_)
{
    struct bst_bufmon *row = bst_bufmon_cast(row_);

    ovs_assert(inited);
    free(row->counter_value);
}

static void
bst_bufmon_unparse_counter_vendor_specific_info(struct ovsdb_idl_row *row_)
{
    struct bst_bufmon *row = bst_bufmon_cast(row_);

    ovs_assert(inited);
    smap_destroy(&row->counter_vendor_specific_info);
}

static void
bst_bufmon_unparse_enabled(struct ovsdb_idl_row *row OVS_UNUSED)
{
    /* Nothing to do. */
}

static void
bst_bufmon_unparse_hw_unit_id(struct ovsdb_idl_row *row OVS_UNUSED)
{
    /* Nothing to do. */
}

static void
bst_bufmon_unparse_name(struct ovsdb_idl_row *row OVS_UNUSED)
{
    /* Nothing to do. */
}

static void
bst_bufmon_unparse_status(struct ovsdb_idl_row *row OVS_UNUSED)
{
    /* Nothing to do. */
}

static void
bst_bufmon_unparse_trigger_threshold(struct ovsdb_idl_row *row_)
{
    struct bst_bufmon *row = bst_bufmon_cast(row_);

    ovs_assert(inited);
    free(row->trigger_threshold);
}

static void
bst_bufmon_init__(struct ovsdb_idl_row *row)
{
    bst_bufmon_init(bst_bufmon_cast(row));
}

/* Clears the contents of 'row' in table "bufmon". */
void
bst_bufmon_init(struct bst_bufmon *row)
{
    memset(row, 0, sizeof *row); 
    smap_init(&row->counter_vendor_specific_info);
}

/* Searches table "bufmon" in 'idl' for a row with UUID 'uuid'.  Returns
 * a pointer to the row if there is one, otherwise a null pointer.  */
const struct bst_bufmon *
bst_bufmon_get_for_uuid(const struct ovsdb_idl *idl, const struct uuid *uuid)
{
    return bst_bufmon_cast(ovsdb_idl_get_row_for_uuid(idl, &bst_table_classes[BST_TABLE_BUFMON], uuid));
}

/* Returns a row in table "bufmon" in 'idl', or a null pointer if that
 * table is empty.
 *
 * Database tables are internally maintained as hash tables, so adding or
 * removing rows while traversing the same table can cause some rows to be
 * visited twice or not at apply. */
const struct bst_bufmon *
bst_bufmon_first(const struct ovsdb_idl *idl)
{
    return bst_bufmon_cast(ovsdb_idl_first_row(idl, &bst_table_classes[BST_TABLE_BUFMON]));
}

/* Returns a row following 'row' within its table, or a null pointer if 'row'
 * is the last row in its table. */
const struct bst_bufmon *
bst_bufmon_next(const struct bst_bufmon *row)
{
    return bst_bufmon_cast(ovsdb_idl_next_row(&row->header_));
}

/* Deletes 'row' from table "bufmon".  'row' may be freed, so it must not be
 * accessed afterward.
 *
 * The caller must have started a transaction with ovsdb_idl_txn_create(). */
void
bst_bufmon_delete(const struct bst_bufmon *row)
{
    ovsdb_idl_txn_delete(&row->header_);
}

/* Inserts and returns a new row in the table "bufmon" in the database
 * with open transaction 'txn'.
 *
 * The new row is assigned a randomly generated provisional UUID.
 * ovsdb-server will assign a different UUID when 'txn' is committed,
 * but the IDL will replace any uses of the provisional UUID in the
 * data to be to be committed by the UUID assigned by ovsdb-server. */
struct bst_bufmon *
bst_bufmon_insert(struct ovsdb_idl_txn *txn)
{
    return bst_bufmon_cast(ovsdb_idl_txn_insert(txn, &bst_table_classes[BST_TABLE_BUFMON], NULL));
}

/* Causes the original contents of column "counter_value" in 'row' to be
 * verified as a prerequisite to completing the transaction.  That is, if
 * "counter_value" in 'row' changed (or if 'row' was deleted) between the
 * time that the IDL originally read its contents and the time that the
 * transaction commits, then the transaction aborts and ovsdb_idl_txn_commit()
 * returns TXN_AGAIN_WAIT or TXN_AGAIN_NOW (depending on whether the database
 * change has already been received).
 *
 * The intention is that, to ensure that no transaction commits based on dirty
 * reads, an application should call this function any time "counter_value" is
 * read as part of a read-modify-write operation.
 *
 * In some cases this function reduces to a no-op, because the current value
 * of "counter_value" is already known:
 *
 *   - If 'row' is a row created by the current transaction (returned by
 *     bst_bufmon_insert()).
 *
 *   - If "counter_value" has already been modified (with
 *     bst_bufmon_set_counter_value()) within the current transaction.
 *
 * Because of the latter property, always call this function *before*
 * bst_bufmon_set_counter_value() for a given read-modify-write.
 *
 * The caller must have started a transaction with ovsdb_idl_txn_create(). */
void
bst_bufmon_verify_counter_value(const struct bst_bufmon *row)
{
    ovs_assert(inited);
    ovsdb_idl_txn_verify(&row->header_, &bst_bufmon_columns[BST_BUFMON_COL_COUNTER_VALUE]);
}

/* Causes the original contents of column "counter_vendor_specific_info" in 'row' to be
 * verified as a prerequisite to completing the transaction.  That is, if
 * "counter_vendor_specific_info" in 'row' changed (or if 'row' was deleted) between the
 * time that the IDL originally read its contents and the time that the
 * transaction commits, then the transaction aborts and ovsdb_idl_txn_commit()
 * returns TXN_AGAIN_WAIT or TXN_AGAIN_NOW (depending on whether the database
 * change has already been received).
 *
 * The intention is that, to ensure that no transaction commits based on dirty
 * reads, an application should call this function any time "counter_vendor_specific_info" is
 * read as part of a read-modify-write operation.
 *
 * In some cases this function reduces to a no-op, because the current value
 * of "counter_vendor_specific_info" is already known:
 *
 *   - If 'row' is a row created by the current transaction (returned by
 *     bst_bufmon_insert()).
 *
 *   - If "counter_vendor_specific_info" has already been modified (with
 *     bst_bufmon_set_counter_vendor_specific_info()) within the current transaction.
 *
 * Because of the latter property, always call this function *before*
 * bst_bufmon_set_counter_vendor_specific_info() for a given read-modify-write.
 *
 * The caller must have started a transaction with ovsdb_idl_txn_create(). */
void
bst_bufmon_verify_counter_vendor_specific_info(const struct bst_bufmon *row)
{
    ovs_assert(inited);
    ovsdb_idl_txn_verify(&row->header_, &bst_bufmon_columns[BST_BUFMON_COL_COUNTER_VENDOR_SPECIFIC_INFO]);
}

/* Causes the original contents of column "enabled" in 'row' to be
 * verified as a prerequisite to completing the transaction.  That is, if
 * "enabled" in 'row' changed (or if 'row' was deleted) between the
 * time that the IDL originally read its contents and the time that the
 * transaction commits, then the transaction aborts and ovsdb_idl_txn_commit()
 * returns TXN_AGAIN_WAIT or TXN_AGAIN_NOW (depending on whether the database
 * change has already been received).
 *
 * The intention is that, to ensure that no transaction commits based on dirty
 * reads, an application should call this function any time "enabled" is
 * read as part of a read-modify-write operation.
 *
 * In some cases this function reduces to a no-op, because the current value
 * of "enabled" is already known:
 *
 *   - If 'row' is a row created by the current transaction (returned by
 *     bst_bufmon_insert()).
 *
 *   - If "enabled" has already been modified (with
 *     bst_bufmon_set_enabled()) within the current transaction.
 *
 * Because of the latter property, always call this function *before*
 * bst_bufmon_set_enabled() for a given read-modify-write.
 *
 * The caller must have started a transaction with ovsdb_idl_txn_create(). */
void
bst_bufmon_verify_enabled(const struct bst_bufmon *row)
{
    ovs_assert(inited);
    ovsdb_idl_txn_verify(&row->header_, &bst_bufmon_columns[BST_BUFMON_COL_ENABLED]);
}

/* Causes the original contents of column "hw_unit_id" in 'row' to be
 * verified as a prerequisite to completing the transaction.  That is, if
 * "hw_unit_id" in 'row' changed (or if 'row' was deleted) between the
 * time that the IDL originally read its contents and the time that the
 * transaction commits, then the transaction aborts and ovsdb_idl_txn_commit()
 * returns TXN_AGAIN_WAIT or TXN_AGAIN_NOW (depending on whether the database
 * change has already been received).
 *
 * The intention is that, to ensure that no transaction commits based on dirty
 * reads, an application should call this function any time "hw_unit_id" is
 * read as part of a read-modify-write operation.
 *
 * In some cases this function reduces to a no-op, because the current value
 * of "hw_unit_id" is already known:
 *
 *   - If 'row' is a row created by the current transaction (returned by
 *     bst_bufmon_insert()).
 *
 *   - If "hw_unit_id" has already been modified (with
 *     bst_bufmon_set_hw_unit_id()) within the current transaction.
 *
 * Because of the latter property, always call this function *before*
 * bst_bufmon_set_hw_unit_id() for a given read-modify-write.
 *
 * The caller must have started a transaction with ovsdb_idl_txn_create(). */
void
bst_bufmon_verify_hw_unit_id(const struct bst_bufmon *row)
{
    ovs_assert(inited);
    ovsdb_idl_txn_verify(&row->header_, &bst_bufmon_columns[BST_BUFMON_COL_HW_UNIT_ID]);
}

/* Causes the original contents of column "name" in 'row' to be
 * verified as a prerequisite to completing the transaction.  That is, if
 * "name" in 'row' changed (or if 'row' was deleted) between the
 * time that the IDL originally read its contents and the time that the
 * transaction commits, then the transaction aborts and ovsdb_idl_txn_commit()
 * returns TXN_AGAIN_WAIT or TXN_AGAIN_NOW (depending on whether the database
 * change has already been received).
 *
 * The intention is that, to ensure that no transaction commits based on dirty
 * reads, an application should call this function any time "name" is
 * read as part of a read-modify-write operation.
 *
 * In some cases this function reduces to a no-op, because the current value
 * of "name" is already known:
 *
 *   - If 'row' is a row created by the current transaction (returned by
 *     bst_bufmon_insert()).
 *
 *   - If "name" has already been modified (with
 *     bst_bufmon_set_name()) within the current transaction.
 *
 * Because of the latter property, always call this function *before*
 * bst_bufmon_set_name() for a given read-modify-write.
 *
 * The caller must have started a transaction with ovsdb_idl_txn_create(). */
void
bst_bufmon_verify_name(const struct bst_bufmon *row)
{
    ovs_assert(inited);
    ovsdb_idl_txn_verify(&row->header_, &bst_bufmon_columns[BST_BUFMON_COL_NAME]);
}

/* Causes the original contents of column "status" in 'row' to be
 * verified as a prerequisite to completing the transaction.  That is, if
 * "status" in 'row' changed (or if 'row' was deleted) between the
 * time that the IDL originally read its contents and the time that the
 * transaction commits, then the transaction aborts and ovsdb_idl_txn_commit()
 * returns TXN_AGAIN_WAIT or TXN_AGAIN_NOW (depending on whether the database
 * change has already been received).
 *
 * The intention is that, to ensure that no transaction commits based on dirty
 * reads, an application should call this function any time "status" is
 * read as part of a read-modify-write operation.
 *
 * In some cases this function reduces to a no-op, because the current value
 * of "status" is already known:
 *
 *   - If 'row' is a row created by the current transaction (returned by
 *     bst_bufmon_insert()).
 *
 *   - If "status" has already been modified (with
 *     bst_bufmon_set_status()) within the current transaction.
 *
 * Because of the latter property, always call this function *before*
 * bst_bufmon_set_status() for a given read-modify-write.
 *
 * The caller must have started a transaction with ovsdb_idl_txn_create(). */
void
bst_bufmon_verify_status(const struct bst_bufmon *row)
{
    ovs_assert(inited);
    ovsdb_idl_txn_verify(&row->header_, &bst_bufmon_columns[BST_BUFMON_COL_STATUS]);
}

/* Causes the original contents of column "trigger_threshold" in 'row' to be
 * verified as a prerequisite to completing the transaction.  That is, if
 * "trigger_threshold" in 'row' changed (or if 'row' was deleted) between the
 * time that the IDL originally read its contents and the time that the
 * transaction commits, then the transaction aborts and ovsdb_idl_txn_commit()
 * returns TXN_AGAIN_WAIT or TXN_AGAIN_NOW (depending on whether the database
 * change has already been received).
 *
 * The intention is that, to ensure that no transaction commits based on dirty
 * reads, an application should call this function any time "trigger_threshold" is
 * read as part of a read-modify-write operation.
 *
 * In some cases this function reduces to a no-op, because the current value
 * of "trigger_threshold" is already known:
 *
 *   - If 'row' is a row created by the current transaction (returned by
 *     bst_bufmon_insert()).
 *
 *   - If "trigger_threshold" has already been modified (with
 *     bst_bufmon_set_trigger_threshold()) within the current transaction.
 *
 * Because of the latter property, always call this function *before*
 * bst_bufmon_set_trigger_threshold() for a given read-modify-write.
 *
 * The caller must have started a transaction with ovsdb_idl_txn_create(). */
void
bst_bufmon_verify_trigger_threshold(const struct bst_bufmon *row)
{
    ovs_assert(inited);
    ovsdb_idl_txn_verify(&row->header_, &bst_bufmon_columns[BST_BUFMON_COL_TRIGGER_THRESHOLD]);
}

/* Returns the "counter_value" column's value from the "bufmon" table in 'row'
 * as a struct ovsdb_datum.  This is useful occasionally: for example,
 * ovsdb_datum_find_key() is an easier and more efficient way to search
 * for a given key than implementing the same operation on the "cooked"
 * form in 'row'.
 *
 * 'key_type' must be OVSDB_TYPE_INTEGER.
 * (This helps to avoid silent bugs if someone changes counter_value's
 * type without updating the caller.)
 *
 * The caller must not modify or free the returned value.
 *
 * Various kinds of changes can invalidate the returned value: modifying
 * 'column' within 'row', deleting 'row', or completing an ongoing transaction.
 * If the returned value is needed for a long time, it is best to make a copy
 * of it with ovsdb_datum_clone().
 *
 * This function is rarely useful, since it is easier to access the value
 * directly through the "counter_value" member in bst_bufmon. */
const struct ovsdb_datum *
bst_bufmon_get_counter_value(const struct bst_bufmon *row,
	enum ovsdb_atomic_type key_type OVS_UNUSED)
{
    ovs_assert(key_type == OVSDB_TYPE_INTEGER);
    return ovsdb_idl_read(&row->header_, &bst_bufmon_col_counter_value);
}

/* Returns the "counter_vendor_specific_info" column's value from the "bufmon" table in 'row'
 * as a struct ovsdb_datum.  This is useful occasionally: for example,
 * ovsdb_datum_find_key() is an easier and more efficient way to search
 * for a given key than implementing the same operation on the "cooked"
 * form in 'row'.
 *
 * 'key_type' must be OVSDB_TYPE_STRING.
 * 'value_type' must be OVSDB_TYPE_STRING.
 * (This helps to avoid silent bugs if someone changes counter_vendor_specific_info's
 * type without updating the caller.)
 *
 * The caller must not modify or free the returned value.
 *
 * Various kinds of changes can invalidate the returned value: modifying
 * 'column' within 'row', deleting 'row', or completing an ongoing transaction.
 * If the returned value is needed for a long time, it is best to make a copy
 * of it with ovsdb_datum_clone().
 *
 * This function is rarely useful, since it is easier to access the value
 * directly through the "counter_vendor_specific_info" member in bst_bufmon. */
const struct ovsdb_datum *
bst_bufmon_get_counter_vendor_specific_info(const struct bst_bufmon *row,
	enum ovsdb_atomic_type key_type OVS_UNUSED,
	enum ovsdb_atomic_type value_type OVS_UNUSED)
{
    ovs_assert(key_type == OVSDB_TYPE_STRING);
    ovs_assert(value_type == OVSDB_TYPE_STRING);
    return ovsdb_idl_read(&row->header_, &bst_bufmon_col_counter_vendor_specific_info);
}

/* Returns the "enabled" column's value from the "bufmon" table in 'row'
 * as a struct ovsdb_datum.  This is useful occasionally: for example,
 * ovsdb_datum_find_key() is an easier and more efficient way to search
 * for a given key than implementing the same operation on the "cooked"
 * form in 'row'.
 *
 * 'key_type' must be OVSDB_TYPE_BOOLEAN.
 * (This helps to avoid silent bugs if someone changes enabled's
 * type without updating the caller.)
 *
 * The caller must not modify or free the returned value.
 *
 * Various kinds of changes can invalidate the returned value: modifying
 * 'column' within 'row', deleting 'row', or completing an ongoing transaction.
 * If the returned value is needed for a long time, it is best to make a copy
 * of it with ovsdb_datum_clone().
 *
 * This function is rarely useful, since it is easier to access the value
 * directly through the "enabled" member in bst_bufmon. */
const struct ovsdb_datum *
bst_bufmon_get_enabled(const struct bst_bufmon *row,
	enum ovsdb_atomic_type key_type OVS_UNUSED)
{
    ovs_assert(key_type == OVSDB_TYPE_BOOLEAN);
    return ovsdb_idl_read(&row->header_, &bst_bufmon_col_enabled);
}

/* Returns the "hw_unit_id" column's value from the "bufmon" table in 'row'
 * as a struct ovsdb_datum.  This is useful occasionally: for example,
 * ovsdb_datum_find_key() is an easier and more efficient way to search
 * for a given key than implementing the same operation on the "cooked"
 * form in 'row'.
 *
 * 'key_type' must be OVSDB_TYPE_INTEGER.
 * (This helps to avoid silent bugs if someone changes hw_unit_id's
 * type without updating the caller.)
 *
 * The caller must not modify or free the returned value.
 *
 * Various kinds of changes can invalidate the returned value: modifying
 * 'column' within 'row', deleting 'row', or completing an ongoing transaction.
 * If the returned value is needed for a long time, it is best to make a copy
 * of it with ovsdb_datum_clone().
 *
 * This function is rarely useful, since it is easier to access the value
 * directly through the "hw_unit_id" member in bst_bufmon. */
const struct ovsdb_datum *
bst_bufmon_get_hw_unit_id(const struct bst_bufmon *row,
	enum ovsdb_atomic_type key_type OVS_UNUSED)
{
    ovs_assert(key_type == OVSDB_TYPE_INTEGER);
    return ovsdb_idl_read(&row->header_, &bst_bufmon_col_hw_unit_id);
}

/* Returns the "name" column's value from the "bufmon" table in 'row'
 * as a struct ovsdb_datum.  This is useful occasionally: for example,
 * ovsdb_datum_find_key() is an easier and more efficient way to search
 * for a given key than implementing the same operation on the "cooked"
 * form in 'row'.
 *
 * 'key_type' must be OVSDB_TYPE_STRING.
 * (This helps to avoid silent bugs if someone changes name's
 * type without updating the caller.)
 *
 * The caller must not modify or free the returned value.
 *
 * Various kinds of changes can invalidate the returned value: modifying
 * 'column' within 'row', deleting 'row', or completing an ongoing transaction.
 * If the returned value is needed for a long time, it is best to make a copy
 * of it with ovsdb_datum_clone().
 *
 * This function is rarely useful, since it is easier to access the value
 * directly through the "name" member in bst_bufmon. */
const struct ovsdb_datum *
bst_bufmon_get_name(const struct bst_bufmon *row,
	enum ovsdb_atomic_type key_type OVS_UNUSED)
{
    ovs_assert(key_type == OVSDB_TYPE_STRING);
    return ovsdb_idl_read(&row->header_, &bst_bufmon_col_name);
}

/* Returns the "status" column's value from the "bufmon" table in 'row'
 * as a struct ovsdb_datum.  This is useful occasionally: for example,
 * ovsdb_datum_find_key() is an easier and more efficient way to search
 * for a given key than implementing the same operation on the "cooked"
 * form in 'row'.
 *
 * 'key_type' must be OVSDB_TYPE_STRING.
 * (This helps to avoid silent bugs if someone changes status's
 * type without updating the caller.)
 *
 * The caller must not modify or free the returned value.
 *
 * Various kinds of changes can invalidate the returned value: modifying
 * 'column' within 'row', deleting 'row', or completing an ongoing transaction.
 * If the returned value is needed for a long time, it is best to make a copy
 * of it with ovsdb_datum_clone().
 *
 * This function is rarely useful, since it is easier to access the value
 * directly through the "status" member in bst_bufmon. */
const struct ovsdb_datum *
bst_bufmon_get_status(const struct bst_bufmon *row,
	enum ovsdb_atomic_type key_type OVS_UNUSED)
{
    ovs_assert(key_type == OVSDB_TYPE_STRING);
    return ovsdb_idl_read(&row->header_, &bst_bufmon_col_status);
}

/* Returns the "trigger_threshold" column's value from the "bufmon" table in 'row'
 * as a struct ovsdb_datum.  This is useful occasionally: for example,
 * ovsdb_datum_find_key() is an easier and more efficient way to search
 * for a given key than implementing the same operation on the "cooked"
 * form in 'row'.
 *
 * 'key_type' must be OVSDB_TYPE_INTEGER.
 * (This helps to avoid silent bugs if someone changes trigger_threshold's
 * type without updating the caller.)
 *
 * The caller must not modify or free the returned value.
 *
 * Various kinds of changes can invalidate the returned value: modifying
 * 'column' within 'row', deleting 'row', or completing an ongoing transaction.
 * If the returned value is needed for a long time, it is best to make a copy
 * of it with ovsdb_datum_clone().
 *
 * This function is rarely useful, since it is easier to access the value
 * directly through the "trigger_threshold" member in bst_bufmon. */
const struct ovsdb_datum *
bst_bufmon_get_trigger_threshold(const struct bst_bufmon *row,
	enum ovsdb_atomic_type key_type OVS_UNUSED)
{
    ovs_assert(key_type == OVSDB_TYPE_INTEGER);
    return ovsdb_idl_read(&row->header_, &bst_bufmon_col_trigger_threshold);
}

/* Sets the "counter_value" column from the "bufmon" table in 'row' to
 * the 'counter_value' set with 'n_counter_value' entries.
 *
 * 'n_counter_value' may be 0 or 1; if it is 0, then 'counter_value'
 * may be NULL.
 *
 * Argument constraints: at least 0
 *
 * The caller retains ownership of the arguments. */
void
bst_bufmon_set_counter_value(const struct bst_bufmon *row, const int64_t *counter_value, size_t n_counter_value)
{
    struct ovsdb_datum datum;
    union ovsdb_atom key;

    ovs_assert(inited);
    if (n_counter_value) {
        datum.n = 1;
        datum.keys = &key;
        key.integer = *counter_value;
    } else {
        datum.n = 0;
        datum.keys = NULL;
    }
    datum.values = NULL;
    ovsdb_idl_txn_write_clone(&row->header_, &bst_bufmon_columns[BST_BUFMON_COL_COUNTER_VALUE], &datum);
}

/* Sets the "counter_vendor_specific_info" column's value from the "bufmon" table in 'row'
 * to 'counter_vendor_specific_info'.
 *
 * The caller retains ownership of 'counter_vendor_specific_info' and everything in it. */
void
bst_bufmon_set_counter_vendor_specific_info(const struct bst_bufmon *row, const struct smap *counter_vendor_specific_info)
{
    struct ovsdb_datum datum;

    ovs_assert(inited);
    if (counter_vendor_specific_info) {
        struct smap_node *node;
        size_t i;

        datum.n = smap_count(counter_vendor_specific_info);
        datum.keys = xmalloc(datum.n * sizeof *datum.keys);
        datum.values = xmalloc(datum.n * sizeof *datum.values);

        i = 0;
        SMAP_FOR_EACH (node, counter_vendor_specific_info) {
            datum.keys[i].string = xstrdup(node->key);
            datum.values[i].string = xstrdup(node->value);
            i++;
        }
        ovsdb_datum_sort_unique(&datum, OVSDB_TYPE_STRING, OVSDB_TYPE_STRING);
    } else {
        ovsdb_datum_init_empty(&datum);
    }
    ovsdb_idl_txn_write(&row->header_,
                        &bst_bufmon_columns[BST_BUFMON_COL_COUNTER_VENDOR_SPECIFIC_INFO],
                        &datum);
}


/* Sets the "enabled" column from the "bufmon" table in 'row' to
 * 'enabled'.
 *
 * The caller retains ownership of the arguments. */
void
bst_bufmon_set_enabled(const struct bst_bufmon *row, bool enabled)
{
    struct ovsdb_datum datum;
    union ovsdb_atom key;

    ovs_assert(inited);
    datum.n = 1;
    datum.keys = &key;
    key.boolean = enabled;
    datum.values = NULL;
    ovsdb_idl_txn_write_clone(&row->header_, &bst_bufmon_columns[BST_BUFMON_COL_ENABLED], &datum);
}

/* Sets the "hw_unit_id" column from the "bufmon" table in 'row' to
 * 'hw_unit_id'.
 *
 * The caller retains ownership of the arguments. */
void
bst_bufmon_set_hw_unit_id(const struct bst_bufmon *row, int64_t hw_unit_id)
{
    struct ovsdb_datum datum;
    union ovsdb_atom key;

    ovs_assert(inited);
    datum.n = 1;
    datum.keys = &key;
    key.integer = hw_unit_id;
    datum.values = NULL;
    ovsdb_idl_txn_write_clone(&row->header_, &bst_bufmon_columns[BST_BUFMON_COL_HW_UNIT_ID], &datum);
}

/* Sets the "name" column from the "bufmon" table in 'row' to
 * 'name'.
 *
 * The caller retains ownership of the arguments. */
void
bst_bufmon_set_name(const struct bst_bufmon *row, const char *name)
{
    struct ovsdb_datum datum;
    union ovsdb_atom key;

    ovs_assert(inited);
    datum.n = 1;
    datum.keys = &key;
    key.string = CONST_CAST(char *, name);
    datum.values = NULL;
    ovsdb_idl_txn_write_clone(&row->header_, &bst_bufmon_columns[BST_BUFMON_COL_NAME], &datum);
}

/* Sets the "status" column from the "bufmon" table in 'row' to
 * the 'status' set.
 *
 * If "status" is null, the column will be the empty set,
 * otherwise it will contain the specified value.
 *
 * Argument constraints: one of "ok", "not properly configured", or "triggered"
 *
 * The caller retains ownership of the arguments. */
void
bst_bufmon_set_status(const struct bst_bufmon *row, const char *status)
{
    struct ovsdb_datum datum;
    union ovsdb_atom key;

    ovs_assert(inited);
    if (status) {
        datum.n = 1;
        datum.keys = &key;
        key.string = CONST_CAST(char *, status);
    } else {
        datum.n = 0;
        datum.keys = NULL;
    }
    datum.values = NULL;
    ovsdb_idl_txn_write_clone(&row->header_, &bst_bufmon_columns[BST_BUFMON_COL_STATUS], &datum);
}

/* Sets the "trigger_threshold" column from the "bufmon" table in 'row' to
 * the 'trigger_threshold' set with 'n_trigger_threshold' entries.
 *
 * 'n_trigger_threshold' may be 0 or 1; if it is 0, then 'trigger_threshold'
 * may be NULL.
 *
 * Argument constraints: at least 0
 *
 * The caller retains ownership of the arguments. */
void
bst_bufmon_set_trigger_threshold(const struct bst_bufmon *row, const int64_t *trigger_threshold, size_t n_trigger_threshold)
{
    struct ovsdb_datum datum;
    union ovsdb_atom key;

    ovs_assert(inited);
    if (n_trigger_threshold) {
        datum.n = 1;
        datum.keys = &key;
        key.integer = *trigger_threshold;
    } else {
        datum.n = 0;
        datum.keys = NULL;
    }
    datum.values = NULL;
    ovsdb_idl_txn_write_clone(&row->header_, &bst_bufmon_columns[BST_BUFMON_COL_TRIGGER_THRESHOLD], &datum);
}

struct ovsdb_idl_column bst_bufmon_columns[BST_BUFMON_N_COLUMNS];

static void
bst_bufmon_columns_init(void)
{
    struct ovsdb_idl_column *c;

    /* Initialize bst_bufmon_col_counter_value. */
    c = &bst_bufmon_col_counter_value;
    c->name = "counter_value";
    ovsdb_base_type_init(&c->type.key, OVSDB_TYPE_INTEGER);
    c->type.key.u.integer.min = INT64_C(0);
    ovsdb_base_type_init(&c->type.value, OVSDB_TYPE_VOID);
    c->type.n_min = 0;
    c->type.n_max = 1;
    c->mutable = true;
    c->parse = bst_bufmon_parse_counter_value;
    c->unparse = bst_bufmon_unparse_counter_value;

    /* Initialize bst_bufmon_col_counter_vendor_specific_info. */
    c = &bst_bufmon_col_counter_vendor_specific_info;
    c->name = "counter_vendor_specific_info";
    ovsdb_base_type_init(&c->type.key, OVSDB_TYPE_STRING);
    c->type.key.u.string.minLen = 0;
    ovsdb_base_type_init(&c->type.value, OVSDB_TYPE_STRING);
    c->type.value.u.string.minLen = 0;
    c->type.n_min = 0;
    c->type.n_max = UINT_MAX;
    c->mutable = true;
    c->parse = bst_bufmon_parse_counter_vendor_specific_info;
    c->unparse = bst_bufmon_unparse_counter_vendor_specific_info;

    /* Initialize bst_bufmon_col_enabled. */
    c = &bst_bufmon_col_enabled;
    c->name = "enabled";
    ovsdb_base_type_init(&c->type.key, OVSDB_TYPE_BOOLEAN);
    ovsdb_base_type_init(&c->type.value, OVSDB_TYPE_VOID);
    c->type.n_min = 1;
    c->type.n_max = 1;
    c->mutable = true;
    c->parse = bst_bufmon_parse_enabled;
    c->unparse = bst_bufmon_unparse_enabled;

    /* Initialize bst_bufmon_col_hw_unit_id. */
    c = &bst_bufmon_col_hw_unit_id;
    c->name = "hw_unit_id";
    ovsdb_base_type_init(&c->type.key, OVSDB_TYPE_INTEGER);
    ovsdb_base_type_init(&c->type.value, OVSDB_TYPE_VOID);
    c->type.n_min = 1;
    c->type.n_max = 1;
    c->mutable = true;
    c->parse = bst_bufmon_parse_hw_unit_id;
    c->unparse = bst_bufmon_unparse_hw_unit_id;

    /* Initialize bst_bufmon_col_name. */
    c = &bst_bufmon_col_name;
    c->name = "name";
    ovsdb_base_type_init(&c->type.key, OVSDB_TYPE_STRING);
    c->type.key.u.string.minLen = 0;
    ovsdb_base_type_init(&c->type.value, OVSDB_TYPE_VOID);
    c->type.n_min = 1;
    c->type.n_max = 1;
    c->mutable = true;
    c->parse = bst_bufmon_parse_name;
    c->unparse = bst_bufmon_unparse_name;

    /* Initialize bst_bufmon_col_status. */
    c = &bst_bufmon_col_status;
    c->name = "status";
    ovsdb_base_type_init(&c->type.key, OVSDB_TYPE_STRING);
    c->type.key.enum_ = xmalloc(sizeof *c->type.key.enum_);
    c->type.key.enum_->n = 3;
    c->type.key.enum_->keys = xmalloc(3 * sizeof *c->type.key.enum_->keys);
    c->type.key.enum_->keys[0].string = xstrdup("not properly configured");
    c->type.key.enum_->keys[1].string = xstrdup("ok");
    c->type.key.enum_->keys[2].string = xstrdup("triggered");
    c->type.key.enum_->values = NULL;
    ovsdb_datum_sort_assert(c->type.key.enum_, OVSDB_TYPE_STRING);
    c->type.key.u.string.minLen = 0;
    ovsdb_base_type_init(&c->type.value, OVSDB_TYPE_VOID);
    c->type.n_min = 0;
    c->type.n_max = 1;
    c->mutable = true;
    c->parse = bst_bufmon_parse_status;
    c->unparse = bst_bufmon_unparse_status;

    /* Initialize bst_bufmon_col_trigger_threshold. */
    c = &bst_bufmon_col_trigger_threshold;
    c->name = "trigger_threshold";
    ovsdb_base_type_init(&c->type.key, OVSDB_TYPE_INTEGER);
    c->type.key.u.integer.min = INT64_C(0);
    ovsdb_base_type_init(&c->type.value, OVSDB_TYPE_VOID);
    c->type.n_min = 0;
    c->type.n_max = 1;
    c->mutable = true;
    c->parse = bst_bufmon_parse_trigger_threshold;
    c->unparse = bst_bufmon_unparse_trigger_threshold;
}

struct ovsdb_idl_table_class bst_table_classes[BST_N_TABLES] = {
    {"System", true,
     bst_system_columns, ARRAY_SIZE(bst_system_columns),
     sizeof(struct bst_system), bst_system_init__},
    {"bufmon", true,
     bst_bufmon_columns, ARRAY_SIZE(bst_bufmon_columns),
     sizeof(struct bst_bufmon), bst_bufmon_init__},
};

struct ovsdb_idl_class bst_idl_class = {
    "bufmon", bst_table_classes, ARRAY_SIZE(bst_table_classes)
};

void
bst_init(void)
{
    if (inited) {
        return;
    }
    assert_single_threaded();
    inited = true;

    bst_system_columns_init();
    bst_bufmon_columns_init();
}

/* Return the schema version.  The caller must not free the returned value. */
const char *
bst_get_db_version(void)
{
    return "0.1.0";
}

