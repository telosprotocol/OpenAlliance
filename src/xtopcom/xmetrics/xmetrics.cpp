// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xmetrics.h"

NS_BEG2(top, metrics)

#define RETURN_METRICS_NAME(TAG) case TAG: return #TAG
char const * matrics_name(xmetrics_tag_t const tag) noexcept {
    switch (tag) {
        RETURN_METRICS_NAME(e_simple_begin);
        RETURN_METRICS_NAME(blockstore_cache_block_total);
        RETURN_METRICS_NAME(vhost_recv_msg);
        RETURN_METRICS_NAME(vhost_recv_callback);
        RETURN_METRICS_NAME(vnode_recv_msg);
        RETURN_METRICS_NAME(vnode_recv_callback);

        // dataobject
        RETURN_METRICS_NAME(dataobject_cons_transaction);
        RETURN_METRICS_NAME(dataobject_block_lightunit);
        RETURN_METRICS_NAME(dataobject_block_fullunit);
        RETURN_METRICS_NAME(dataobject_block_lighttable);
        RETURN_METRICS_NAME(dataobject_block_fulltable);
        RETURN_METRICS_NAME(dataobject_block_empty);
        RETURN_METRICS_NAME(dataobject_tx_receipt_t);
        RETURN_METRICS_NAME(dataobject_unit_state);
        RETURN_METRICS_NAME(dataobject_table_state);
        RETURN_METRICS_NAME(dataobject_bstate_ctx);
        RETURN_METRICS_NAME(dataobject_xvtxindex);
        RETURN_METRICS_NAME(dataobject_xvbstate);
        RETURN_METRICS_NAME(dataobject_xvproperty);
        RETURN_METRICS_NAME(dataobject_xvaccountobj);
        RETURN_METRICS_NAME(dataobject_exeunit);
        RETURN_METRICS_NAME(dataobject_exegroup);
        RETURN_METRICS_NAME(dataobject_xvexecontxt);
        RETURN_METRICS_NAME(dataobject_xaccount_index);
        RETURN_METRICS_NAME(dataobject_xreceiptid_pair_t);
        RETURN_METRICS_NAME(dataobject_xvbindex_t);
        RETURN_METRICS_NAME(dataobject_xtransaction_t);
        RETURN_METRICS_NAME(dataobject_provcert);
        RETURN_METRICS_NAME(dataobject_xvaccount);
        RETURN_METRICS_NAME(dataobject_xvaction);
        RETURN_METRICS_NAME(dataobject_xvheader);
        RETURN_METRICS_NAME(dataobject_xvqcert);
        RETURN_METRICS_NAME(dataobject_xvblock);
        RETURN_METRICS_NAME(dataobject_xvinput);
        RETURN_METRICS_NAME(dataobject_xvoutput);
        RETURN_METRICS_NAME(dataobject_xventity);
        // vledger dataobject
        RETURN_METRICS_NAME(dataobject_xvnode_t);
        RETURN_METRICS_NAME(dataobject_xvexestate_t);
        RETURN_METRICS_NAME(dataobject_xvnodegroup);
        RETURN_METRICS_NAME(dataobject_xcscoreobj_t);
        RETURN_METRICS_NAME(dataobject_xblock_maker_t);
        RETURN_METRICS_NAME(dataobject_xblockacct_t);
        RETURN_METRICS_NAME(dataobject_xtxpool_table_info_t);
        RETURN_METRICS_NAME(dataobject_xacctmeta_t);
        RETURN_METRICS_NAME(dataobject_account_address);
        RETURN_METRICS_NAME(dataobject_mpt_state_object);
        RETURN_METRICS_NAME(dataobject_mpt_trie_node_cnt);

        // dbkeys
        RETURN_METRICS_NAME(db_key_tx);
        RETURN_METRICS_NAME(db_key_block_index);
        RETURN_METRICS_NAME(db_key_block);
        RETURN_METRICS_NAME(db_key_block_object);
        RETURN_METRICS_NAME(db_key_block_input);
        RETURN_METRICS_NAME(db_key_block_input_resource);
        RETURN_METRICS_NAME(db_key_block_output);
        RETURN_METRICS_NAME(db_key_block_output_resource);
        RETURN_METRICS_NAME(db_key_block_state);
        RETURN_METRICS_NAME(db_read);
        RETURN_METRICS_NAME(db_write);
        RETURN_METRICS_NAME(db_delete);
        RETURN_METRICS_NAME(db_delete_range);
        RETURN_METRICS_NAME(db_read_size);
        RETURN_METRICS_NAME(db_write_size);
        RETURN_METRICS_NAME(db_read_tick);
        RETURN_METRICS_NAME(db_write_tick);
        RETURN_METRICS_NAME(db_delete_tick);
        RETURN_METRICS_NAME(db_block_cache_size);
        RETURN_METRICS_NAME(db_memtable_cache_size);
        RETURN_METRICS_NAME(db_memory_total_size);

        // consensus
        RETURN_METRICS_NAME(cons_drand_leader_finish_succ);
        RETURN_METRICS_NAME(cons_drand_backup_finish_succ);
        RETURN_METRICS_NAME(cons_drand_leader_finish_fail);
        RETURN_METRICS_NAME(cons_drand_backup_finish_fail);
        RETURN_METRICS_NAME(cons_tableblock_leader_finish_succ);
        RETURN_METRICS_NAME(cons_tableblock_backup_finish_succ);
        RETURN_METRICS_NAME(cons_tableblock_leader_finish_fail);
        RETURN_METRICS_NAME(cons_tableblock_backup_finish_fail);
        RETURN_METRICS_NAME(cons_drand_leader_succ);
        RETURN_METRICS_NAME(cons_drand_backup_succ);
        RETURN_METRICS_NAME(cons_tableblock_leader_succ);
        RETURN_METRICS_NAME(cons_tableblock_backup_succ);
        RETURN_METRICS_NAME(cons_tableblock_total_succ);
        RETURN_METRICS_NAME(cons_pacemaker_tc_discontinuity);

        RETURN_METRICS_NAME(cons_table_leader_make_proposal_succ);
        RETURN_METRICS_NAME(cons_table_backup_verify_proposal_succ);
        RETURN_METRICS_NAME(cons_fail_make_proposal_table_state);
        RETURN_METRICS_NAME(cons_fail_make_proposal_consensus_para);
        RETURN_METRICS_NAME(cons_fail_verify_proposal_blocks_invalid);
        RETURN_METRICS_NAME(cons_fail_verify_proposal_table_state_get);
        RETURN_METRICS_NAME(cons_fail_verify_proposal_drand_invalid);
        RETURN_METRICS_NAME(cons_fail_verify_proposal_consensus_para_get);
        RETURN_METRICS_NAME(cons_fail_verify_proposal_unit_count);
        RETURN_METRICS_NAME(cons_fail_make_proposal_table_check_latest_state);
        RETURN_METRICS_NAME(cons_fail_verify_proposal_table_check_latest_state);
        RETURN_METRICS_NAME(cons_fail_verify_proposal_table_with_local);
        RETURN_METRICS_NAME(cons_fail_verify_proposal_rise_confirm_id);
        RETURN_METRICS_NAME(cons_fail_verify_proposal_confirm_id_error);
        RETURN_METRICS_NAME(cons_fail_make_proposal_unit_check_state);
        RETURN_METRICS_NAME(cons_fail_make_proposal_view_changed);
        RETURN_METRICS_NAME(cons_fail_vote_not_enough);
        RETURN_METRICS_NAME(cons_view_fire_clock_delay);
        RETURN_METRICS_NAME(cons_view_fire_succ);
        RETURN_METRICS_NAME(cons_cp_check_succ);
        RETURN_METRICS_NAME(cons_state_check_succ);
        RETURN_METRICS_NAME(cons_view_fire_is_leader);
        RETURN_METRICS_NAME(cons_fail_backup_view_not_match);
        RETURN_METRICS_NAME(cons_make_proposal_tick);
        RETURN_METRICS_NAME(cons_verify_proposal_tick);
        RETURN_METRICS_NAME(cons_make_fulltable_tick);
        RETURN_METRICS_NAME(cons_make_lighttable_tick);
        RETURN_METRICS_NAME(cons_verify_lighttable_tick);
        RETURN_METRICS_NAME(cons_make_unit_tick);
        RETURN_METRICS_NAME(cons_unitbuilder_lightunit_tick);
        RETURN_METRICS_NAME(cons_unitbuilder_fullunit_tick);
        RETURN_METRICS_NAME(cons_unitmaker_check_state_tick);
        RETURN_METRICS_NAME(cons_tablebuilder_lighttable_tick);
        RETURN_METRICS_NAME(cons_tablebuilder_fulltable_tick);
        RETURN_METRICS_NAME(cons_tablemaker_verify_proposal_tick);
        RETURN_METRICS_NAME(cons_tablemaker_make_proposal_tick);
        RETURN_METRICS_NAME(cons_tablemaker_check_state_tick);
        RETURN_METRICS_NAME(cons_tablemaker_refresh_cache);

        RETURN_METRICS_NAME(cons_table_leader_get_txpool_tx_count);
        RETURN_METRICS_NAME(cons_table_leader_get_txpool_sendtx_count);
        RETURN_METRICS_NAME(cons_table_leader_get_txpool_recvtx_count);
        RETURN_METRICS_NAME(cons_table_leader_get_txpool_confirmtx_count);
        RETURN_METRICS_NAME(cons_table_leader_make_tx_count);
        RETURN_METRICS_NAME(cons_table_leader_make_unit_count);
        RETURN_METRICS_NAME(cons_table_total_process_tx_count);
        RETURN_METRICS_NAME(cons_table_total_process_unit_count);

        RETURN_METRICS_NAME(cons_packtx_succ);
        RETURN_METRICS_NAME(cons_packtx_sendtx_succ);
        RETURN_METRICS_NAME(cons_packtx_recvtx_succ);
        RETURN_METRICS_NAME(cons_packtx_confirmtx_succ);
        RETURN_METRICS_NAME(cons_packtx_fail_unit_check_state);
        RETURN_METRICS_NAME(cons_packtx_fail_fullunit_limit);
        RETURN_METRICS_NAME(cons_packtx_fail_receiptid_continuous);
        RETURN_METRICS_NAME(cons_packtx_fail_rspid_continuous);
        RETURN_METRICS_NAME(cons_packtx_fail_total_unconfirm_limit);
        RETURN_METRICS_NAME(cons_packtx_fail_table_unconfirm_limit);
        RETURN_METRICS_NAME(cons_packtx_fail_nonce_continuous);
        RETURN_METRICS_NAME(cons_packtx_fail_transfer_limit);
        RETURN_METRICS_NAME(cons_packtx_fail_load_origintx);
        RETURN_METRICS_NAME(cons_packtx_with_threshold);
        RETURN_METRICS_NAME(cons_invoke_sync_state_count);
        RETURN_METRICS_NAME(cons_invoke_sync_block_count);

        RETURN_METRICS_NAME(clock_aggregate_height);
        RETURN_METRICS_NAME(clock_leader_broadcast_height);
        RETURN_METRICS_NAME(clock_received_height);

        // store
        RETURN_METRICS_NAME(store_state_read);
        RETURN_METRICS_NAME(store_state_table_write);
        RETURN_METRICS_NAME(store_state_unit_write);
        RETURN_METRICS_NAME(store_state_delete);
        RETURN_METRICS_NAME(store_block_table_read);
        RETURN_METRICS_NAME(store_block_unit_read);
        RETURN_METRICS_NAME(store_block_other_read);
        RETURN_METRICS_NAME(store_block_index_read);
        RETURN_METRICS_NAME(store_block_input_read);
        RETURN_METRICS_NAME(store_block_output_read);
        RETURN_METRICS_NAME(store_block_call);
        RETURN_METRICS_NAME(store_block_table_write);
        RETURN_METRICS_NAME(store_block_unit_write);
        RETURN_METRICS_NAME(store_block_other_write);
        RETURN_METRICS_NAME(store_block_index_table_write);
        RETURN_METRICS_NAME(store_block_index_unit_write);
        RETURN_METRICS_NAME(store_block_index_other_write);
        RETURN_METRICS_NAME(store_block_input_table_write);
        RETURN_METRICS_NAME(store_block_input_unit_write);
        RETURN_METRICS_NAME(store_block_output_table_write);
        RETURN_METRICS_NAME(store_block_output_unit_write);
        RETURN_METRICS_NAME(store_block_delete);
        RETURN_METRICS_NAME(store_tx_index_self);
        RETURN_METRICS_NAME(store_tx_index_send);
        RETURN_METRICS_NAME(store_tx_index_recv);
        RETURN_METRICS_NAME(store_tx_index_confirm);
        RETURN_METRICS_NAME(store_tx_origin);
        RETURN_METRICS_NAME(store_block_meta_write);
        RETURN_METRICS_NAME(store_block_meta_read);

        RETURN_METRICS_NAME(store_dbsize_block_unit_empty);
        RETURN_METRICS_NAME(store_dbsize_block_unit_light);
        RETURN_METRICS_NAME(store_dbsize_block_unit_full);
        RETURN_METRICS_NAME(store_dbsize_block_table_empty);
        RETURN_METRICS_NAME(store_dbsize_block_table_light);
        RETURN_METRICS_NAME(store_dbsize_block_table_full);
        RETURN_METRICS_NAME(store_dbsize_block_other);

        // message category
        RETURN_METRICS_NAME(message_category_consensus_contains_duplicate);
        RETURN_METRICS_NAME(message_category_timer_contains_duplicate);
        RETURN_METRICS_NAME(message_category_txpool_contains_duplicate);
        RETURN_METRICS_NAME(message_category_rpc_contains_duplicate);
        RETURN_METRICS_NAME(message_category_sync_contains_duplicate);
        RETURN_METRICS_NAME(message_category_state_sync_contains_duplicate);
        RETURN_METRICS_NAME(message_block_broadcast_contains_duplicate);
        RETURN_METRICS_NAME(message_category_relay_contains_duplicate);
        RETURN_METRICS_NAME(message_category_unknown_contains_duplicate);

        RETURN_METRICS_NAME(message_category_recv);
        RETURN_METRICS_NAME(message_category_consensus);
        RETURN_METRICS_NAME(message_category_timer);
        RETURN_METRICS_NAME(message_category_txpool);
        RETURN_METRICS_NAME(message_category_rpc);
        RETURN_METRICS_NAME(message_category_sync);
        RETURN_METRICS_NAME(message_category_state_sync);
        RETURN_METRICS_NAME(message_block_broadcast);
        RETURN_METRICS_NAME(message_category_relay);
        RETURN_METRICS_NAME(message_category_unknown);

        RETURN_METRICS_NAME(message_category_send);
        RETURN_METRICS_NAME(message_send_category_consensus);
        RETURN_METRICS_NAME(message_send_category_timer);
        RETURN_METRICS_NAME(message_send_category_txpool);
        RETURN_METRICS_NAME(message_send_category_rpc);
        RETURN_METRICS_NAME(message_send_category_sync);
        RETURN_METRICS_NAME(message_send_block_broadcast);
        RETURN_METRICS_NAME(message_send_category_unknown);

        RETURN_METRICS_NAME(message_category_broad);
        RETURN_METRICS_NAME(message_broad_category_consensus);
        RETURN_METRICS_NAME(message_broad_category_timer);
        RETURN_METRICS_NAME(message_broad_category_txpool);
        RETURN_METRICS_NAME(message_broad_category_rpc);
        RETURN_METRICS_NAME(message_broad_category_sync);
        RETURN_METRICS_NAME(message_broad_block_broadcast);
        RETURN_METRICS_NAME(message_broad_category_unknown);

        RETURN_METRICS_NAME(message_category_rumor);
        RETURN_METRICS_NAME(message_rumor_category_consensus);
        RETURN_METRICS_NAME(message_rumor_category_timer);
        RETURN_METRICS_NAME(message_rumor_category_txpool);
        RETURN_METRICS_NAME(message_rumor_category_rpc);
        RETURN_METRICS_NAME(message_rumor_category_sync);
        RETURN_METRICS_NAME(message_rumor_block_broadcast);
        RETURN_METRICS_NAME(message_rumor_category_unknown);

        RETURN_METRICS_NAME(message_transport_recv);
        RETURN_METRICS_NAME(message_transport_send);

        // sync
        RETURN_METRICS_NAME(xsync_recv_new_block);
        RETURN_METRICS_NAME(xsync_recv_new_hash);
        RETURN_METRICS_NAME(xsync_recv_invalid_block);
        RETURN_METRICS_NAME(xsync_recv_duplicate_block);
        RETURN_METRICS_NAME(xsync_recv_block_size);
        RETURN_METRICS_NAME(xsync_getblocks_recv_req);
        RETURN_METRICS_NAME(xsync_getblocks_send_resp);
        RETURN_METRICS_NAME(xsync_recv_broadcast_newblockhash);
        RETURN_METRICS_NAME(xsync_recv_blocks);
        RETURN_METRICS_NAME(xsync_recv_blocks_size);
        RETURN_METRICS_NAME(xsync_handler_blocks);
        RETURN_METRICS_NAME(xsync_recv_gossip_recv);
        RETURN_METRICS_NAME(xsync_bytes_gossip_recv);
        RETURN_METRICS_NAME(xsync_recv_get_on_demand_blocks);
        RETURN_METRICS_NAME(xsync_recv_get_on_demand_blocks_bytes);
        RETURN_METRICS_NAME(xsync_recv_on_demand_blocks);
        RETURN_METRICS_NAME(xsync_recv_on_demand_blocks_bytes);
        RETURN_METRICS_NAME(xsync_recv_broadcast_chain_state);
        RETURN_METRICS_NAME(xsync_recv_broadcast_chain_state_bytes);
        RETURN_METRICS_NAME(xsync_recv_response_chain_state);
        RETURN_METRICS_NAME(xsync_recv_response_chain_state_bytes);
        RETURN_METRICS_NAME(xsync_recv_cross_cluster_chain_state);
        RETURN_METRICS_NAME(xsync_recv_cross_cluster_chain_state_bytes);
        RETURN_METRICS_NAME(xsync_recv_blocks_by_hashes);
        RETURN_METRICS_NAME(xsync_recv_blocks_by_hashes_bytes);
        RETURN_METRICS_NAME(xsync_handler_blocks_by_hashes);
        RETURN_METRICS_NAME(xsync_cost_role_add_event);
        RETURN_METRICS_NAME(xsync_cost_role_remove_event);
        RETURN_METRICS_NAME(xsync_handle_chain_snapshot_request);
        RETURN_METRICS_NAME(xsync_handler_chain_snapshot_reponse);
        RETURN_METRICS_NAME(xsync_handle_ondemand_chain_snapshot_request);
        RETURN_METRICS_NAME(xsync_handle_ondemand_chain_snapshot_reponse);
        RETURN_METRICS_NAME(xsync_recv_on_demand_by_hash_blocks_req);
        RETURN_METRICS_NAME(xsync_recv_on_demand_by_hash_blocks_req_bytes);
        RETURN_METRICS_NAME(xsync_recv_on_demand_by_hash_blocks_resp);
        RETURN_METRICS_NAME(xsync_recv_on_demand_by_hash_blocks_resp_bytes);
        RETURN_METRICS_NAME(xsync_behind_download);
        RETURN_METRICS_NAME(xsync_behind_check);
        RETURN_METRICS_NAME(xsync_behind_on_demand);
        RETURN_METRICS_NAME(xsync_behind_on_demand_by_hash);
        RETURN_METRICS_NAME(xsync_recv_get_blocks_by_hashes);
        RETURN_METRICS_NAME(xsync_recv_get_blocks_by_hashes_bytes);
        RETURN_METRICS_NAME(xsync_store_block_units);
        RETURN_METRICS_NAME(xsync_store_block_tables);
        RETURN_METRICS_NAME(xsync_unit_proof_sync_req_send);
        RETURN_METRICS_NAME(xsync_unit_proof_sync_req_recv);
        RETURN_METRICS_NAME(xsync_recv_archive_height);
        RETURN_METRICS_NAME(xsync_archive_height_blocks);
        RETURN_METRICS_NAME(xsync_recv_archive_blocks);
        RETURN_METRICS_NAME(xsync_recv_archive_blocks_size);
        RETURN_METRICS_NAME(xsync_recv_query_archive_height);
        RETURN_METRICS_NAME(xsync_recv_archive_height_list);

        // txpool
        RETURN_METRICS_NAME(txpool_received_self_send_receipt_num);
        RETURN_METRICS_NAME(txpool_received_other_send_receipt_num);
        RETURN_METRICS_NAME(txpool_recv_tx_retry_send);
        RETURN_METRICS_NAME(txpool_confirm_tx_retry_send);
        RETURN_METRICS_NAME(txpool_recv_tx_first_send);
        RETURN_METRICS_NAME(txpool_confirm_tx_first_send);
        RETURN_METRICS_NAME(txpool_request_origin_tx);
        RETURN_METRICS_NAME(txpool_receipt_tx);
        RETURN_METRICS_NAME(txpool_pull_recv_tx);
        RETURN_METRICS_NAME(txpool_pull_confirm_tx);
        RETURN_METRICS_NAME(txpool_push_tx_from_proposal);
        RETURN_METRICS_NAME(txpool_send_tx_cur);
        RETURN_METRICS_NAME(txpool_recv_tx_cur);
        RETURN_METRICS_NAME(txpool_confirm_tx_cur);
        RETURN_METRICS_NAME(txpool_unconfirm_tx_cur);
        RETURN_METRICS_NAME(txpool_recv_tx_first_send_fail);
        RETURN_METRICS_NAME(txpool_confirm_tx_first_send_fail);
        RETURN_METRICS_NAME(txpool_drop_send_receipt_msg);
        RETURN_METRICS_NAME(txpool_drop_receive_receipt_msg);
        RETURN_METRICS_NAME(txpool_drop_push_receipt_msg);
        RETURN_METRICS_NAME(txpool_drop_pull_recv_receipt_msg);
        RETURN_METRICS_NAME(txpool_drop_pull_confirm_receipt_msg_v2);
        RETURN_METRICS_NAME(txpool_drop_receipt_id_state_msg);
        RETURN_METRICS_NAME(txpool_try_sync_table_block);
        RETURN_METRICS_NAME(txpool_receipt_recv_num_by_1_clock);
        RETURN_METRICS_NAME(txpool_receipt_recv_num_by_2_clock);
        RETURN_METRICS_NAME(txpool_receipt_recv_num_by_3_clock);
        RETURN_METRICS_NAME(txpool_receipt_recv_num_by_4_clock);
        RETURN_METRICS_NAME(txpool_receipt_recv_num_by_5_clock);
        RETURN_METRICS_NAME(txpool_receipt_recv_num_by_6_clock);
        RETURN_METRICS_NAME(txpool_receipt_recv_num_7to12_clock);
        RETURN_METRICS_NAME(txpool_receipt_recv_num_13to30_clock);
        RETURN_METRICS_NAME(txpool_receipt_recv_num_exceed_30_clock);
        RETURN_METRICS_NAME(txpool_push_send_fail_table_limit);
        RETURN_METRICS_NAME(txpool_push_send_fail_role_limit);
        RETURN_METRICS_NAME(txpool_push_send_fail_repeat);
        RETURN_METRICS_NAME(txpool_push_send_fail_unconfirm_limit);
        RETURN_METRICS_NAME(txpool_push_send_fail_nonce_limit);
        RETURN_METRICS_NAME(txpool_push_send_fail_account_fall_behind);
        RETURN_METRICS_NAME(txpool_push_send_fail_account_not_in_charge);
        RETURN_METRICS_NAME(txpool_push_send_fail_nonce_expired);
        RETURN_METRICS_NAME(txpool_push_send_fail_nonce_duplicate);
        RETURN_METRICS_NAME(txpool_push_send_fail_replaced);
        RETURN_METRICS_NAME(txpool_push_send_fail_other);
        RETURN_METRICS_NAME(txpool_send_tx_timeout);
        RETURN_METRICS_NAME(txpool_tx_delay_from_push_to_pack_send);
        RETURN_METRICS_NAME(txpool_tx_delay_from_push_to_pack_recv);
        RETURN_METRICS_NAME(txpool_tx_delay_from_push_to_pack_confirm);
        RETURN_METRICS_NAME(txpool_tx_delay_from_push_to_commit_send);
        RETURN_METRICS_NAME(txpool_tx_delay_from_push_to_commit_recv);
        RETURN_METRICS_NAME(txpool_tx_delay_from_push_to_commit_confirm);
        RETURN_METRICS_NAME(txpool_receipt_id_state_msg_send_num);
        RETURN_METRICS_NAME(txpool_alarm_confirm_tx_reached_upper_limit);
        RETURN_METRICS_NAME(txpool_alarm_recv_tx_reached_upper_limit);
        RETURN_METRICS_NAME(txpool_alarm_send_tx_reached_upper_limit);
        RETURN_METRICS_NAME(txpool_sync_on_demand_unit);
        RETURN_METRICS_NAME(txpool_sender_unconfirm_cache);
        RETURN_METRICS_NAME(txpool_receiver_unconfirm_cache);
        RETURN_METRICS_NAME(txpool_height_record_cache);
        RETURN_METRICS_NAME(txpool_table_unconfirm_raw_txs);

        // txstore
        RETURN_METRICS_NAME(txstore_request_origin_tx);
        RETURN_METRICS_NAME(txstore_cache_origin_tx);

        // blockstore
        RETURN_METRICS_NAME(blockstore_index_load);
        RETURN_METRICS_NAME(blockstore_blk_load);

        // blockstore accessing
        RETURN_METRICS_NAME(blockstore_access_from_account_context);
        RETURN_METRICS_NAME(blockstore_access_from_contract_runtime);

        // access from mbus
        RETURN_METRICS_NAME(blockstore_access_from_mbus);
        RETURN_METRICS_NAME(blockstore_access_from_mbus_onchain_loader_t_update);
        RETURN_METRICS_NAME(blockstore_access_from_mbus_contract_db_on_block);
        RETURN_METRICS_NAME(blockstore_access_from_mbus_txpool_db_event_on_block);
        RETURN_METRICS_NAME(blockstore_access_from_mbus_xelect_process_elect);
        RETURN_METRICS_NAME(blockstore_access_from_mbus_grpc_process_event);

        // rpc access
        RETURN_METRICS_NAME(blockstore_access_from_rpc);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_block);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_committed_block);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_chain_info);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_latest_tables);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_cert_blk);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_timer_clock);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_unit);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_block_committed_block);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_block_full_block);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_block_by_height);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_block_load_object);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_block_committed_height);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_block_query_propery);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_block_set_table);
        RETURN_METRICS_NAME(blockstore_access_from_rpc_get_block_json);

        RETURN_METRICS_NAME(blockstore_access_from_store);

        // txpool access
        RETURN_METRICS_NAME(blockstore_access_from_txpool);
        RETURN_METRICS_NAME(blockstore_access_from_txpool_on_block_event);
        RETURN_METRICS_NAME(blockstore_access_from_txpool_id_state);
        RETURN_METRICS_NAME(blockstore_access_from_txpool_get_nonce);
        RETURN_METRICS_NAME(blockstore_access_from_txpool_refresh_table);
        RETURN_METRICS_NAME(blockstore_access_from_txpool_create_receipt);
        RETURN_METRICS_NAME(blockstore_access_from_txpool_pull_lacking_receipts);

        // blockstore access statestore
        RETURN_METRICS_NAME(blockstore_access_from_statestore);
        RETURN_METRICS_NAME(blockstore_access_from_statestore_rebuild_state);
        RETURN_METRICS_NAME(blockstore_access_from_statestore_fullblock);
        RETURN_METRICS_NAME(blockstore_access_from_statestore_load_state);
        RETURN_METRICS_NAME(blockstore_access_from_statestore_load_lastest_state);
        RETURN_METRICS_NAME(blockstore_access_from_statestore_get_block_state);
        RETURN_METRICS_NAME(blockstore_access_from_statestore_get_block_index_state);
        RETURN_METRICS_NAME(blockstore_access_from_statestore_get_connect_state);
        RETURN_METRICS_NAME(blockstore_access_from_statestore_get_commit_state);

        RETURN_METRICS_NAME(blockstore_access_from_application);

        // sync access
        RETURN_METRICS_NAME(blockstore_access_from_sync);
        RETURN_METRICS_NAME(blockstore_access_from_sync_blk);
        RETURN_METRICS_NAME(blockstore_access_from_sync_get_latest_connected_block);
        RETURN_METRICS_NAME(blockstore_access_from_sync_get_latest_committed_block);
        RETURN_METRICS_NAME(blockstore_access_from_sync_get_latest_locked_block);
        RETURN_METRICS_NAME(blockstore_access_from_sync_get_latest_cert_block);
        RETURN_METRICS_NAME(blockstore_access_from_sync_existed_blk);
        RETURN_METRICS_NAME(blockstore_access_from_sync_update_latest_genesis_connected_block);
        RETURN_METRICS_NAME(blockstore_access_from_sync_get_latest_committed_full_block);
        RETURN_METRICS_NAME(blockstore_access_from_sync_get_latest_executed_block);
        RETURN_METRICS_NAME(blockstore_access_from_sync_get_genesis_block);
        RETURN_METRICS_NAME(blockstore_access_from_sync_load_block_objects);
        RETURN_METRICS_NAME(blockstore_access_from_sync_load_block_objects_input);
        RETURN_METRICS_NAME(blockstore_access_from_sync_load_block_objects_output);
        RETURN_METRICS_NAME(blockstore_access_from_sync_load_tx);
        RETURN_METRICS_NAME(blockstore_access_from_sync_load_tx_input);
        RETURN_METRICS_NAME(blockstore_access_from_sync_load_tx_output);
        RETURN_METRICS_NAME(blockstore_access_from_sync_store_blk);
        RETURN_METRICS_NAME(blockstore_access_from_sync_query_blk);
        RETURN_METRICS_NAME(blockstore_access_from_sync_load_block_object);

        RETURN_METRICS_NAME(blockstore_access_from_sync_index);

        // blockstore_access_from_blk_mk
        RETURN_METRICS_NAME(blockstore_access_from_blk_mk);
        RETURN_METRICS_NAME(blockstore_access_from_blk_mk_ld_and_cache);
        RETURN_METRICS_NAME(blockstore_access_from_blk_mk_proposer_verify_proposal);
        RETURN_METRICS_NAME(blockstore_access_from_blk_mk_proposer_verify_proposal_drand);
        RETURN_METRICS_NAME(blockstore_access_from_blk_mk_table);
        RETURN_METRICS_NAME(blockstore_access_from_blk_mk_unit_ld_last_blk);
        RETURN_METRICS_NAME(blockstore_access_from_blk_mk_unit_chk_last_state);

        RETURN_METRICS_NAME(blockstore_access_from_us);
        RETURN_METRICS_NAME(blockstore_access_from_us_on_view_fire);
        RETURN_METRICS_NAME(blockstore_access_from_us_on_timer_fire);
        RETURN_METRICS_NAME(blockstore_access_from_us_on_proposal_finish);
        RETURN_METRICS_NAME(blockstore_access_from_us_timer_blk_maker);
        RETURN_METRICS_NAME(blockstore_access_from_us_timer_picker_constructor);
        RETURN_METRICS_NAME(blockstore_access_from_us_dispatcher_load_tc);

        RETURN_METRICS_NAME(blockstore_access_from_bft);
        RETURN_METRICS_NAME(blockstore_access_from_bft_check_proposal);
        RETURN_METRICS_NAME(blockstore_access_from_bft_on_clock_fire);
        RETURN_METRICS_NAME(blockstore_access_from_bft_pdu_event_down);
        RETURN_METRICS_NAME(blockstore_access_from_bft_consaccnt_on_proposal_start);
        RETURN_METRICS_NAME(blockstore_access_from_bft_consdriv_on_proposal_start);
        RETURN_METRICS_NAME(blockstore_access_from_bft_get_commit_blk);
        RETURN_METRICS_NAME(blockstore_access_from_bft_get_lock_blk);
        RETURN_METRICS_NAME(blockstore_access_from_bft_sync);
        RETURN_METRICS_NAME(blockstore_access_from_bft_init_blk);

        RETURN_METRICS_NAME(blockstore_access_from_vnodesrv);
        RETURN_METRICS_NAME(blockstore_tick);

        // state store
        RETURN_METRICS_NAME(statestore_access);
        RETURN_METRICS_NAME(statestore_access_from_blk_ctx);
        RETURN_METRICS_NAME(statestore_access_from_vledger_load_state);
        RETURN_METRICS_NAME(statestore_access_from_vnodesrv_load_state);
        RETURN_METRICS_NAME(statestore_access_from_store_tgas);
        RETURN_METRICS_NAME(statestore_access_from_store_backup_tgas);
        RETURN_METRICS_NAME(statestore_access_from_store_bstate);
        RETURN_METRICS_NAME(statestore_access_from_xelect);
        RETURN_METRICS_NAME(statestore_access_from_xconfig_update);
        RETURN_METRICS_NAME(statestore_access_from_rpc_get_fullbock);
        RETURN_METRICS_NAME(statestore_access_from_rpc_query_propery);
        RETURN_METRICS_NAME(statestore_access_from_rpc_set_addition);
        RETURN_METRICS_NAME(statestore_access_from_rpc_set_fullunit);
        RETURN_METRICS_NAME(statestore_access_from_sync_chain_snapshot);
        RETURN_METRICS_NAME(statestore_access_from_sync_handle_chain_snapshot_meta);
        RETURN_METRICS_NAME(statestore_access_from_sync_query_offchain);
        RETURN_METRICS_NAME(statestore_access_from_application_isbeacon);
        RETURN_METRICS_NAME(statestore_access_from_application_load_election);
        RETURN_METRICS_NAME(statestore_access_from_blkmaker_update_account_state);
        RETURN_METRICS_NAME(statestore_access_from_blkmaker_unit_connect_state);
        RETURN_METRICS_NAME(statestore_access_from_txpool_get_accountstate);
        RETURN_METRICS_NAME(statestore_access_from_txpool_refreshtable);
        RETURN_METRICS_NAME(statestore_access_from_blockstore);
        RETURN_METRICS_NAME(statestore_access_from_blkmaker_get_target_tablestate);
        RETURN_METRICS_NAME(statestore_access_from_contract_framework);

        RETURN_METRICS_NAME(statestore_get_unit_state_succ);
        RETURN_METRICS_NAME(statestore_get_unit_state_from_cache);
        RETURN_METRICS_NAME(statestore_get_unit_state_from_db);        
        RETURN_METRICS_NAME(statestore_get_unit_state_with_unit_count);
        RETURN_METRICS_NAME(statestore_get_table_state_succ);
        RETURN_METRICS_NAME(statestore_get_table_state_from_cache);
        RETURN_METRICS_NAME(statestore_get_table_state_from_db);
        RETURN_METRICS_NAME(statestore_get_table_state_with_table_count);
        RETURN_METRICS_NAME(statestore_load_table_block_succ);
        RETURN_METRICS_NAME(statestore_execute_block_recursive_succ);
        RETURN_METRICS_NAME(statestore_execute_unit_recursive_succ);        

        RETURN_METRICS_NAME(statestore_sync_succ);

        RETURN_METRICS_NAME(state_load_blk_state_suc);
        RETURN_METRICS_NAME(state_load_blk_state_cache_suc);
        RETURN_METRICS_NAME(state_load_blk_state_fail);
        RETURN_METRICS_NAME(state_load_blk_state_table_suc);
        RETURN_METRICS_NAME(state_load_blk_state_table_fail);
        RETURN_METRICS_NAME(state_load_blk_state_table_cache_suc);
        RETURN_METRICS_NAME(state_load_blk_state_unit_suc);
        RETURN_METRICS_NAME(state_load_blk_state_unit_fail);
        RETURN_METRICS_NAME(state_load_blk_state_unit_cache_suc);
        RETURN_METRICS_NAME(state_delete_table_data);
        RETURN_METRICS_NAME(state_delete_unit_state);
        RETURN_METRICS_NAME(state_delete_mpt);
        RETURN_METRICS_NAME(state_delete_by_full_table);
        RETURN_METRICS_NAME(state_delete_create_mpt_fail);

        RETURN_METRICS_NAME(data_table_unpack_units);
        RETURN_METRICS_NAME(data_table_unpack_one_unit);

        RETURN_METRICS_NAME(txexecutor_total_system_contract_count);
        RETURN_METRICS_NAME(txexecutor_system_contract_failed_count);

        RETURN_METRICS_NAME(xevent_major_type_none);
        RETURN_METRICS_NAME(xevent_major_type_timer);
        RETURN_METRICS_NAME(xevent_major_type_chain_timer);
        RETURN_METRICS_NAME(xevent_major_type_store);
        RETURN_METRICS_NAME(xevent_major_type_sync_executor);
        RETURN_METRICS_NAME(xevent_major_type_network);
        RETURN_METRICS_NAME(xevent_major_type_dispatch);
        RETURN_METRICS_NAME(xevent_major_type_deceit);
        RETURN_METRICS_NAME(xevent_major_type_consensus);
        RETURN_METRICS_NAME(xevent_major_type_transaction);
        RETURN_METRICS_NAME(xevent_major_type_behind);
        RETURN_METRICS_NAME(xevent_major_type_vnode);
        RETURN_METRICS_NAME(xevent_major_type_account);
        RETURN_METRICS_NAME(xevent_major_type_role);
        RETURN_METRICS_NAME(xevent_major_type_blockfetcher);
        RETURN_METRICS_NAME(xevent_major_type_sync);
        RETURN_METRICS_NAME(xevent_major_type_state_sync);

        RETURN_METRICS_NAME(rpc_edge_tx_request);
        RETURN_METRICS_NAME(rpc_edge_query_request);
        RETURN_METRICS_NAME(rpc_auditor_tx_request);
        RETURN_METRICS_NAME(rpc_auditor_query_request);
        RETURN_METRICS_NAME(rpc_auditor_forward_request);
        RETURN_METRICS_NAME(rpc_validator_tx_request);
        RETURN_METRICS_NAME(rpc_query_account_succ);
        // contract
        RETURN_METRICS_NAME(contract_table_fullblock_event);
        RETURN_METRICS_NAME(contract_table_statistic_exec_fullblock);
        RETURN_METRICS_NAME(contract_table_statistic_report_fullblock);
        RETURN_METRICS_NAME(contract_zec_slash_summarize_fullblock);

        RETURN_METRICS_NAME(mailbox_grpc_total);
        RETURN_METRICS_NAME(mailbox_block_fetcher_total);
        RETURN_METRICS_NAME(mailbox_downloader_total);
        RETURN_METRICS_NAME(mailbox_xsync_total);
        RETURN_METRICS_NAME(mailbox_rpc_auditor_total);
        RETURN_METRICS_NAME(mailbox_rpc_validator_total);
        RETURN_METRICS_NAME(mailbox_txpool_fast_total);
        RETURN_METRICS_NAME(mailbox_txpool_slow_total);
        RETURN_METRICS_NAME(mailbox_us_total);
        RETURN_METRICS_NAME(mailbox_statestore_total);
        RETURN_METRICS_NAME(mailbox_grpc_cur);
        RETURN_METRICS_NAME(mailbox_block_fetcher_cur);
        RETURN_METRICS_NAME(mailbox_downloader_cur);
        RETURN_METRICS_NAME(mailbox_xsync_cur);
        RETURN_METRICS_NAME(mailbox_rpc_auditor_cur);
        RETURN_METRICS_NAME(mailbox_rpc_validator_cur);
        RETURN_METRICS_NAME(mailbox_txpool_fast_cur);
        RETURN_METRICS_NAME(mailbox_txpool_slow_cur);
        RETURN_METRICS_NAME(mailbox_us_cur);
        RETURN_METRICS_NAME(mailbox_statestore_cur);

        //txdelay
        RETURN_METRICS_NAME(txdelay_client_timestamp_unmatch);
        RETURN_METRICS_NAME(txdelay_from_client_to_edge);
        RETURN_METRICS_NAME(txdelay_from_client_to_auditor);
        RETURN_METRICS_NAME(txdelay_from_client_to_validator);
        RETURN_METRICS_NAME(txdelay_from_client_to_sendtx_exec);
        RETURN_METRICS_NAME(txdelay_from_client_to_recvtx_exec);
        RETURN_METRICS_NAME(txdelay_from_client_to_confirmtx_exec);

        //cpu
        RETURN_METRICS_NAME(cpu_hash_256_calc);
        RETURN_METRICS_NAME(cpu_hash_64_calc);        
        RETURN_METRICS_NAME(cpu_ca_merge_sign_xbft);
        RETURN_METRICS_NAME(cpu_ca_merge_sign_tc);
        RETURN_METRICS_NAME(cpu_ca_do_sign_xbft);
        RETURN_METRICS_NAME(cpu_ca_do_sign_tc);
        RETURN_METRICS_NAME(cpu_ca_verify_sign_xbft);
        RETURN_METRICS_NAME(cpu_ca_verify_sign_tc);
        RETURN_METRICS_NAME(cpu_ca_verify_multi_sign_txreceipt);
        RETURN_METRICS_NAME(cpu_ca_verify_multi_sign_sync);
        RETURN_METRICS_NAME(cpu_ca_verify_multi_sign_xbft);
        RETURN_METRICS_NAME(cpu_ca_verify_multi_sign_tc);
        RETURN_METRICS_NAME(cpu_ca_verify_multi_sign_blockstore);
        RETURN_METRICS_NAME(cpu_merkle_hash_calc);
        RETURN_METRICS_NAME(cpu_hash_256_xecprikey_calc);
        RETURN_METRICS_NAME(cpu_hash_256_XudpSocket_calc);
        RETURN_METRICS_NAME(cpu_hash_256_GetRootKadmliaKey_calc);
        RETURN_METRICS_NAME(cpu_hash_256_handle_register_node_calc);
        RETURN_METRICS_NAME(cpu_hash_256_xtransaction_v1_calc);
        RETURN_METRICS_NAME(cpu_hash_256_xtransaction_v2_calc);
        RETURN_METRICS_NAME(cpu_hash_256_receiptid_bin_calc);
        RETURN_METRICS_NAME(cpu_hash_256_xvproperty_prove_t_leafs_calc);
        RETURN_METRICS_NAME(cpu_hash_256_xvproperty_property_bin_calc);
        RETURN_METRICS_NAME(cpu_hash_256_xhashplugin_t_calc);

        //bft
        RETURN_METRICS_NAME(bft_verify_vote_msg_fail);
        RETURN_METRICS_NAME(contract_table_statistic_empty_ptr);

        //statectx
        RETURN_METRICS_NAME(statectx_load_block_succ);
        RETURN_METRICS_NAME(statectx_load_state_succ);
        RETURN_METRICS_NAME(statectx_sync_invoke_count);

        RETURN_METRICS_NAME(mpt_total_pruned_trie_node_cnt);
        RETURN_METRICS_NAME(mpt_cached_pruned_trie_node_cnt);
        //prune
        RETURN_METRICS_NAME(prune_block_table);
        RETURN_METRICS_NAME(prune_block_unit);
        RETURN_METRICS_NAME(prune_block_drand);
        RETURN_METRICS_NAME(prune_block_timer);
        RETURN_METRICS_NAME(prune_block_contract);
        RETURN_METRICS_NAME(prune_state_unitstate);

        default: assert(false); return nullptr;
    }
}
#undef RETURN_METRICS_NAME

#define RETURN_METRICS_INFO(TAG,SIZE) case TAG: return {#TAG,SIZE}
std::pair<char const *, std::size_t> array_counter_info(xmetrics_array_tag_t const tag) noexcept {
    switch (tag) {
        RETURN_METRICS_INFO(e_array_counter_begin, 0);

        RETURN_METRICS_INFO(blockstore_sharding_table_block_commit, 64);
        RETURN_METRICS_INFO(blockstore_beacon_table_block_commit, 1);
        RETURN_METRICS_INFO(blockstore_zec_table_block_commit, 3);
        RETURN_METRICS_INFO(blockstore_sharding_table_block_full, 64);
        RETURN_METRICS_INFO(blockstore_beacon_table_block_full, 1);
        RETURN_METRICS_INFO(blockstore_zec_table_block_full, 3);
        RETURN_METRICS_INFO(blockstore_sharding_table_block_genesis_connect, 64);
        RETURN_METRICS_INFO(blockstore_beacon_table_block_genesis_connect, 1);
        RETURN_METRICS_INFO(blockstore_zec_table_block_genesis_connect, 3);
        RETURN_METRICS_INFO(e_array_counter_total, 0);

    default:
        assert(false);
        return {nullptr, 0};
    }
}
#undef RETURN_METRICS_INFO


void e_metrics::start(const std::string& log_path)
{
    top::metrics::handler::metrics_log_init(log_path);
    start();
}

void e_metrics::start() {
    if (running()) {
        return;
    }
    XMETRICS_CONFIG_GET("dump_interval", m_dump_interval);
    XMETRICS_CONFIG_GET("queue_procss_behind_sleep_time", m_queue_procss_behind_sleep_time);

    for (size_t index = e_simple_begin; index < e_simple_total; index++) {
        s_metrics[index] = std::make_shared<metrics_counter_unit>(matrics_name(static_cast<xmetrics_tag_t>(index)), 0);
    }

    for (size_t index = e_array_counter_begin; index < e_array_counter_total; index++) {
        auto name = array_counter_info(static_cast<xmetrics_array_tag_t>(index)).first;
        auto size = array_counter_info(static_cast<xmetrics_array_tag_t>(index)).second;
        a_metrics[index] = std::make_shared<metrics_array_unit>(name, size, 0);
        a_counters[index].arr_count.resize(size, copiable_atomwrapper<uint64_t>(0));
        a_counters[index].arr_value.resize(size, copiable_atomwrapper<int64_t>(0));
    }

    running(true);
    // auto self = shared_from_this();
    // threading::xbackend_thread::spawn([this, self] { run_process(); });
    m_process_thread = std::thread(&e_metrics::run_process, this);
    m_process_thread.detach();
}

void e_metrics::stop() {
    assert(running());
    running(false);
}

void e_metrics::run_process() {
    while (running()) {
        process_message_queue();
        std::this_thread::sleep_for(m_queue_procss_behind_sleep_time);
        update_dump();
    }

    top::metrics::handler::metrics_log_close();

}

void e_metrics::process_message_queue() {
    auto message_v = m_message_queue.wait_and_pop_all();
    if (message_v.empty())
        return;
    if (message_v.size() > 50000) {
        xkinfo("[xmetrics]alarm metrics_queue_size %zu", message_v.size());
    }
    for (auto & msg_event : message_v) {
        if (msg_event.metrics_name.empty())
            continue;
        metrics::metrics_variant_ptr metrics_ptr;
        auto metrics_real_name = msg_event.metrics_name.substr(0, msg_event.metrics_name.find("&0x", 0));
        if (m_metrics_hub.find(metrics_real_name) == m_metrics_hub.end()) {
            switch (msg_event.major_id) {
            case e_metrics_major_id::count:
                metrics_ptr = m_counter_handler.init_new_metrics(msg_event);
                break;
            case e_metrics_major_id::timer:
                metrics_ptr = m_timer_handler.init_new_metrics(msg_event);
                break;
            case e_metrics_major_id::flow:
                metrics_ptr = m_flow_handler.init_new_metrics(msg_event);
                break;
            default:
                assert(false);
                break;
            }
            m_metrics_hub.insert({metrics_real_name, metrics_ptr});
            assert(m_metrics_hub.count(metrics_real_name));
        } else {
            metrics_ptr = m_metrics_hub[metrics_real_name];
            auto index = static_cast<metrics::e_metrics_major_id>(metrics_ptr.GetType());
            switch (index) {
            case metrics::e_metrics_major_id::count:
                m_counter_handler.process_message_event(metrics_ptr, msg_event);
                break;
            case metrics::e_metrics_major_id::timer:
                m_timer_handler.process_message_event(metrics_ptr, msg_event);
                break;
            case metrics::e_metrics_major_id::flow:
                m_flow_handler.process_message_event(metrics_ptr, msg_event);
                break;
            default:
                break;
            }
        }
    }
}

void e_metrics::update_dump() {
    if (--m_dump_interval)
        return;
    auto const & map_ref = m_metrics_hub;
    for (auto const & pair : map_ref) {
        auto const & metrics_ptr = pair.second;
        auto index = static_cast<metrics::e_metrics_major_id>(pair.second.GetType());
        switch (index) {
        case metrics::e_metrics_major_id::count:
            m_counter_handler.dump_metrics_info(metrics_ptr);
            break;
        case metrics::e_metrics_major_id::timer:
            m_timer_handler.dump_metrics_info(metrics_ptr);
            break;
        case metrics::e_metrics_major_id::flow:
            m_flow_handler.dump_metrics_info(metrics_ptr);
            break;
        default:
            assert(false);
            break;
        }
    }

    // simpe metrics dump
    gauge_dump();

    // array_counter metrics dump
    array_count_dump();

    XMETRICS_CONFIG_GET("dump_interval", m_dump_interval);
}
/*
 * one funtion for one [major_id-minor_id] message_event
 */

void e_metrics::timer_start(std::string metrics_name, time_point time) {
    m_message_queue.push(event_message(metrics::e_metrics_major_id::timer, metrics::e_metrics_minor_id::timestart, metrics_name, time));
}
void e_metrics::timer_end(std::string metrics_name, time_point time, metrics_appendant_info info, microseconds timed_out) {
    m_message_queue.push(event_message(metrics::e_metrics_major_id::timer, metrics::e_metrics_minor_id::timeend, metrics_name, time, info, timed_out));
}

void e_metrics::counter_increase(std::string metrics_name, int64_t value) {
    m_message_queue.push(event_message(metrics::e_metrics_major_id::count, metrics::e_metrics_minor_id::increase, metrics_name, value));
}
void e_metrics::counter_decrease(std::string metrics_name, int64_t value) {
    m_message_queue.push(event_message(metrics::e_metrics_major_id::count, metrics::e_metrics_minor_id::decrease, metrics_name, value));
}
void e_metrics::counter_set(std::string metrics_name, int64_t value) {
    m_message_queue.push(event_message(metrics::e_metrics_major_id::count, metrics::e_metrics_minor_id::set, metrics_name, value));
}
void e_metrics::flow_count(std::string metrics_name, int64_t value, time_point timestamp) {
    m_message_queue.push(event_message(metrics::e_metrics_major_id::flow, metrics::e_metrics_minor_id::flow_count, metrics_name, value, metrics_appendant_info{timestamp}));
}
void e_metrics::gauge(E_SIMPLE_METRICS_TAG tag, int64_t value) {
    if (tag >= e_simple_total || tag <= e_simple_begin ) {
        return;
    }
    s_counters[tag].value += value;
    s_counters[tag].call_count++;
}
void e_metrics::gauge_set_value(E_SIMPLE_METRICS_TAG tag, int64_t value) {
    if (tag >= e_simple_total || tag <= e_simple_begin ) {
        return;
    }
    xassert(tag < message_category_send || tag > message_broad_category_end);
    s_counters[tag].value = value;
    s_counters[tag].call_count++;
}
int64_t e_metrics::gauge_get_value(E_SIMPLE_METRICS_TAG tag) {
    if (tag >= e_simple_total || tag <= e_simple_begin ) {
        return 0;
    }
    return s_counters[tag].value;
}

void e_metrics::array_counter_increase(E_ARRAY_COUNTER_TAG tag, std::size_t index, int64_t value) {
    a_counters[tag].arr_count[index]++;
    a_counters[tag].arr_value[index] += value;
}

void e_metrics::array_counter_decrease(E_ARRAY_COUNTER_TAG tag, std::size_t index, int64_t value) {
    a_counters[tag].arr_count[index]++;
    a_counters[tag].arr_value[index] -= value;
}

void e_metrics::array_counter_set(E_ARRAY_COUNTER_TAG tag, std::size_t index, int64_t value) {
    a_counters[tag].arr_count[index]++;
    a_counters[tag].arr_value[index] = value;
}

struct xsimple_merics_category
{
    E_SIMPLE_METRICS_TAG category;
    E_SIMPLE_METRICS_TAG start;
    E_SIMPLE_METRICS_TAG end;
};

// categories
xsimple_merics_category g_cates[] = {
    {blockstore_access_from_txpool, blockstore_access_from_txpool_begin, blockstore_access_from_txpool_end},
    {blockstore_access_from_statestore, blockstore_access_from_statestore_begin, blockstore_access_from_statestore_end},
    {blockstore_access_from_sync, blockstore_access_from_sync_begin, blockstore_access_from_sync_end},
    {blockstore_access_from_blk_mk, blockstore_access_from_blk_mk_begin, blockstore_access_from_blk_mk_end},
    {blockstore_access_from_us, blockstore_access_from_us_begin, blockstore_access_from_us_end},
    {blockstore_access_from_bft, blockstore_access_from_bft_begin, blockstore_access_from_bft_end},
    {statestore_access, statestore_access_begin, statestore_access_end},
    {blockstore_access_from_mbus, blockstore_access_from_mbus_begin, blockstore_access_from_mbus_end},
    {blockstore_access_from_rpc, blockstore_access_from_rpc_begin, blockstore_access_from_rpc_end},
    {message_category_recv, message_category_begin, message_category_end},
    {message_category_send, message_send_category_begin, message_send_category_end},
    {message_category_broad, message_broad_category_begin, message_broad_category_end},
    {message_category_rumor, message_rumor_category_begin, message_rumor_category_end}
};

bool is_category(E_SIMPLE_METRICS_TAG tag) {
    for(size_t index = 0; index < sizeof(g_cates)/sizeof(g_cates[0]); index++) {
        if(tag == g_cates[index].category) {
            return true;
        }
    }
    return false;
}

void e_metrics::gauge_dump() {
    // detail metrics dump
    for(auto index = (int32_t)e_simple_begin + 1; index < (int32_t)e_simple_total; index++) {
        if(is_category((E_SIMPLE_METRICS_TAG)index)) {
            continue;
        }
        auto metrics_ptr = s_metrics[index];
        auto ptr = metrics_ptr.GetRef<metrics_counter_unit_ptr>();
        ptr->inner_val = s_counters[index].value;
        ptr->count = s_counters[index].call_count;
        m_counter_handler.dump_metrics_info(ptr);
    }

    // summary of category as defined
    for(size_t index = 0; index < sizeof(g_cates)/sizeof(g_cates[0]); index++) {
        uint64_t cate_val = 0;
        uint64_t cate_count = 0;
        auto cate = g_cates[index];
        for(auto cate_index = (int)cate.start; cate_index <= (int)cate.end; cate_index++) {
            cate_val += s_counters[cate_index].value;
            cate_count += s_counters[cate_index].call_count;
        }
        auto metrics_ptr = s_metrics[cate.category];
        auto ptr = metrics_ptr.GetRef<metrics_counter_unit_ptr>();
        ptr->inner_val = cate_val;
        ptr->count = cate_count;
        m_counter_handler.dump_metrics_info(ptr);
    }
}

void e_metrics::array_count_dump() {
    for (auto index = e_array_counter_begin + 1; index < e_array_counter_total; index++) {
        auto metrics_ptr = a_metrics[index];
        auto ptr = metrics_ptr.GetRef<metrics_array_unit_ptr>();
        ptr->all_count = 0;
        for (std::size_t _i = 0; _i < a_counters[index].arr_count.size(); ++_i) {
            ptr->all_count += a_counters[index].arr_count[_i].val();
            ptr->array_count[_i] = a_counters[index].arr_count[_i].val();
            ptr->array_value[_i] = a_counters[index].arr_value[_i].val();
        }
        m_array_counter_handler.dump_metrics_info(ptr);
    }
}

NS_END2
