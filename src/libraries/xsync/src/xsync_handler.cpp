// Copyright (c) 2017-2018 Telos Foundation & contributors
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "xsync/xsync_handler.h"
#include <inttypes.h>
#include "xsync/xsync_log.h"
#include "xbase/xutl.h"
#include "xvnetwork/xaddress.h"
#include "xsync/xgossip_message.h"
#include "xmetrics/xmetrics.h"
#include "xconfig/xconfig_register.h"
#include "xsync/xdeceit_node_manager.h"
#include "xvnetwork/xmessage.h"
#include "xsync/xsync_message.h"
#include "xdata/xnative_contract_address.h"
#include "xblockstore/xblockstore_face.h"
#include "xsync/xsync_util.h"
#include "xsync/xsync_sender.h"
#include "xmbus/xevent_role.h"
#include "xmbus/xevent_executor.h"
#include "xmbus/xevent_blockfetcher.h"
#include "xbasic/xutility.h"
#include "xsyncbase/xmessage_ids.h"
#include "xdata/xblock.h"
#include "xdata/xfull_tableblock.h"
#include "xdata/xtable_bstate.h"
#include "xsync/xsync_prune.h"

NS_BEG2(top, sync)

using namespace base;
using namespace mbus;
using namespace vnetwork;
using namespace data;

xsync_handler_t::xsync_handler_t(std::string vnode_id,
    xsync_store_face_t *sync_store,
    const observer_ptr<base::xvcertauth_t> &certauth,
    xsync_session_manager_t *session_mgr, xdeceit_node_manager_t *blacklist,
    xrole_chains_mgr_t *role_chains_mgr, xrole_xips_manager_t *role_xips_mgr,
    xdownloader_t *downloader, xblock_fetcher_t *block_fetcher,
    xsync_gossip_t *sync_gossip,
    xsync_pusher_t *sync_pusher,
    xsync_broadcast_t *sync_broadcast,
    xsync_sender_t *sync_sender,
    xsync_on_demand_t *sync_on_demand,
    xsync_peerset_t *peerset, xsync_peer_keeper_t *peer_keeper, xsync_behind_checker_t *behind_checker,
    xsync_cross_cluster_chain_state_t *cross_cluster_chain_state):
m_vnode_id(vnode_id),
m_sync_store(sync_store),
m_certauth(certauth),
m_session_mgr(session_mgr),
m_blacklist(blacklist),
m_role_chains_mgr(role_chains_mgr),
m_role_xips_mgr(role_xips_mgr),
m_downloader(downloader),
m_block_fetcher(block_fetcher),
m_sync_gossip(sync_gossip),
m_sync_pusher(sync_pusher),
m_sync_broadcast(sync_broadcast),
m_sync_sender(sync_sender),
m_sync_on_demand(sync_on_demand),
m_peerset(peerset),
m_peer_keeper(peer_keeper),
m_behind_checker(behind_checker),
m_cross_cluster_chain_state(cross_cluster_chain_state) {

    register_handler(xmessage_id_sync_gossip, std::bind(&xsync_handler_t::gossip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
    register_handler(xmessage_id_sync_frozen_gossip, std::bind(&xsync_handler_t::gossip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
    register_handler(xmessage_id_sync_broadcast_chain_state, std::bind(&xsync_handler_t::broadcast_chain_state, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
    register_handler(xmessage_id_sync_frozen_broadcast_chain_state, std::bind(&xsync_handler_t::broadcast_chain_state, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
    register_handler(xmessage_id_sync_response_chain_state, std::bind(&xsync_handler_t::response_chain_state, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
    register_handler(xmessage_id_sync_frozen_response_chain_state, std::bind(&xsync_handler_t::response_chain_state, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
    register_handler(xmessage_id_sync_cross_cluster_chain_state, std::bind(&xsync_handler_t::cross_cluster_chain_state, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
    register_handler(xmessage_id_sync_chain_snapshot_request, std::bind(&xsync_handler_t::handle_chain_snapshot_request, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
    register_handler(xmessage_id_sync_chain_snapshot_response, std::bind(&xsync_handler_t::handle_chain_snapshot_response, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
    register_handler(xmessage_id_sync_ondemand_chain_snapshot_request, std::bind(&xsync_handler_t::handle_ondemand_chain_snapshot_request, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
    register_handler(xmessage_id_sync_ondemand_chain_snapshot_response, std::bind(&xsync_handler_t::handle_ondemand_chain_snapshot_response, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
    register_handler(xmessage_id_sync_query_archive_height, std::bind(&xsync_handler_t::recv_query_archive_height, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));  

    register_handler(xmessage_id_sync_newblock_push, std::bind(&xsync_handler_t::on_block_push_newblock, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
    register_handler(xmessage_id_sync_block_request,  std::bind(&xsync_handler_t::on_block_request_process, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
    register_handler(xmessage_id_sync_block_response, std::bind(&xsync_handler_t::on_block_response_process, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
    
    if(!m_sync_store->is_sync_protocal_forked()){
        register_handler(xmessage_id_sync_archive_height, std::bind(&xsync_handler_t::recv_archive_height, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_get_blocks, std::bind(&xsync_handler_t::get_blocks, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_blocks, std::bind(&xsync_handler_t::blocks, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_push_newblock, std::bind(&xsync_handler_t::push_newblock, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_push_newblockhash, std::bind(&xsync_handler_t::push_newblockhash, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_broadcast_newblockhash, std::bind(&xsync_handler_t::broadcast_newblockhash, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_on_demand_blocks, std::bind(&xsync_handler_t::on_demand_blocks, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_get_on_demand_blocks, std::bind(&xsync_handler_t::get_on_demand_blocks, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_get_blocks_by_hashes, std::bind(&xsync_handler_t::get_blocks_by_hashes, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_blocks_by_hashes, std::bind(&xsync_handler_t::blocks_by_hashes, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_get_on_demand_by_hash_blocks, std::bind(&xsync_handler_t::get_on_demand_by_hash_blocks, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_on_demand_by_hash_blocks, std::bind(&xsync_handler_t::on_demand_by_hash_blocks, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_get_on_demand_blocks_with_proof, std::bind(&xsync_handler_t::get_on_demand_blocks_with_proof, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_on_demand_blocks_with_proof, std::bind(&xsync_handler_t::on_demand_blocks_with_proof, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_archive_blocks, std::bind(&xsync_handler_t::recv_archive_blocks, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_get_on_demand_blocks_with_hash, std::bind(&xsync_handler_t::get_on_demand_blocks_with_hash, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_on_demand_blocks_with_hash, std::bind(&xsync_handler_t::on_demand_blocks_with_hash, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
        register_handler(xmessage_id_sync_archive_height_list, std::bind(&xsync_handler_t::recv_archive_height_list, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7));
   
        register_unused_handler(xmessage_id_sync_get_blocks);
        register_unused_handler(xmessage_id_sync_blocks);
        register_unused_handler(xmessage_id_sync_push_newblock);
        register_unused_handler(xmessage_id_sync_push_newblockhash);
        register_unused_handler(xmessage_id_sync_broadcast_newblockhash);
        register_unused_handler(xmessage_id_sync_on_demand_blocks);
        register_unused_handler(xmessage_id_sync_get_on_demand_blocks);
        register_unused_handler(xmessage_id_sync_get_blocks_by_hashes);
        register_unused_handler(xmessage_id_sync_blocks_by_hashes);
        register_unused_handler(xmessage_id_sync_get_on_demand_by_hash_blocks);
        register_unused_handler(xmessage_id_sync_on_demand_by_hash_blocks);
        register_unused_handler(xmessage_id_sync_get_on_demand_blocks_with_proof);
        register_unused_handler(xmessage_id_sync_on_demand_blocks_with_proof);
        register_unused_handler(xmessage_id_sync_archive_blocks);
        register_unused_handler(xmessage_id_sync_get_on_demand_blocks_with_hash);
        register_unused_handler(xmessage_id_sync_on_demand_blocks_with_hash);
        register_unused_handler(xmessage_id_sync_archive_height_list);
    } 
}

xsync_handler_t::~xsync_handler_t() {
}

void xsync_handler_t::on_message(
    const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xbyte_buffer_t &msg,
    xmessage_t::message_type msg_type,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    uint32_t msg_size = msg.size();

    base::xstream_t stream(base::xcontext_t::instance(), (uint8_t*)msg.data(), msg.size());

    xsync_message_header_ptr_t header = make_object_ptr<xsync_message_header_t>();
    header->serialize_from(stream);

    if (header->code == (uint8_t)xsync_msg_err_code_t::decode) {
        xsync_warn("xsync_handler on_message decode failed %" PRIx64 " %s", msg_hash, from_address.to_string().c_str());
        return;
    }

    if(!reject_hander_msg_after_block_forked(msg_type)) {
        xsync_warn("xsync_handler on_message(unrecognized) %" PRIx64 " %s reject after forked.", msg_hash, from_address.to_string().c_str());
        return;
    }
    
    auto it = m_handlers.find(msg_type);
    if (it != m_handlers.end()) {
        xsync_handler_netmsg_callback cb = it->second;
        cb(msg_size, from_address, network_self, header, stream, msg_hash, recv_time);
    } else {
        xsync_warn("xsync_handler on_message(unrecognized) %" PRIx64 " %s", msg_hash, from_address.to_string().c_str());
    }
}

void xsync_handler_t::on_event(const mbus::xevent_ptr_t& e) {

    m_downloader->push_event(e);

    if (e->major_type == mbus::xevent_major_type_timer) {
        m_session_mgr->on_timer();
        m_sync_gossip->on_timer();
        m_behind_checker->on_timer();
        m_peer_keeper->on_timer();
        m_cross_cluster_chain_state->on_timer();
        m_sync_pusher->on_timer();
    }

    if (e->major_type == mbus::xevent_major_type_chain_timer) {
        m_sync_gossip->on_chain_timer(e);
    }

    if (e->major_type==mbus::xevent_major_type_behind) {

        if (e->minor_type == mbus::xevent_behind_t::type_check)
            m_behind_checker->on_behind_check_event(e);
        else if (e->minor_type == mbus::xevent_behind_t::type_on_demand)
            m_sync_on_demand->on_behind_event(e);
        else if (e->minor_type == mbus::xevent_behind_t::type_on_demand_by_hash)
            m_sync_on_demand->on_behind_by_hash_event(e);
    }

    if (e->major_type == mbus::xevent_major_type_role) {
        handle_role_change(e);
    }

    if (e->major_type == mbus::xevent_major_type_consensus) {
        handle_consensus_result(e);
    }
}

void xsync_handler_t::get_blocks(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {
    
    XMETRICS_GAUGE(metrics::xsync_getblocks_recv_req, 1);
    auto ptr = make_object_ptr<xsync_message_get_blocks_t>();
    ptr->serialize_from(stream);

    const std::string &owner = ptr->owner;
    uint64_t start_height = ptr->start_height;
    uint32_t count = ptr->count;

    if (owner == "" || start_height == 0 || count == 0)
        return;

    xsync_info("xsync_handler receive getblocks %" PRIx64 " wait(%ldms) %s range[%lu,%lu] %s",
        msg_hash, get_time()-recv_time, owner.c_str(), start_height, start_height+count-1, from_address.to_string().c_str());

    std::vector<xblock_ptr_t> vector_blocks;
    for (uint32_t height = start_height, i = 0; height < start_height + count && i < max_request_block_count; height++) {
        auto blocks = m_sync_store->load_block_objects(owner, height);
        if (blocks.empty()) {
            break;
        }
        for (uint32_t j = 0; j < blocks.size(); j++,i++){
            vector_blocks.push_back(xblock_t::raw_vblock_to_object_ptr(blocks[j].get()));
        }
    }
    XMETRICS_GAUGE(metrics::xsync_getblocks_send_resp, vector_blocks.size());
    m_sync_sender->send_blocks(xsync_msg_err_code_t::succ, owner, vector_blocks, network_self, from_address);
}

void xsync_handler_t::push_newblock(uint32_t msg_size,
        const vnetwork::xvnode_address_t &from_address,
        const vnetwork::xvnode_address_t &network_self,
        const xsync_message_header_ptr_t &header,
        base::xstream_t &stream,
        xtop_vnetwork_message::hash_result_type msg_hash,
        int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_new_block, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_block_size, msg_size);

    auto ptr = make_object_ptr<xsync_message_push_newblock_t>();
    ptr->serialize_from(stream);

    if (ptr->block == nullptr)
        return;

    xblock_ptr_t &block = ptr->block;

    if (!(common::has<common::xnode_type_t::storage>(network_self.type()) || (common::has<common::xnode_type_t::fullnode>(network_self.type())))) {
        xsync_warn("xsync_handler receive push_newblock(target must be archive or fullnode) %" PRIx64 " %s %s %s",
            msg_hash, block->dump().c_str(), network_self.to_string().c_str(), from_address.to_string().c_str());
        XMETRICS_GAUGE(metrics::xsync_recv_invalid_block, 1);
        return;
    }

    if (!common::has<common::xnode_type_t::rec>(from_address.type()) && !common::has<common::xnode_type_t::zec>(from_address.type()) &&
        !common::has<common::xnode_type_t::consensus>(from_address.type()) && !common::has<common::xnode_type_t::evm>(from_address.type()) &&
        !common::has<common::xnode_type_t::relay>(from_address.type())) {
        xsync_warn("xsync_handler receive push_newblock(source must be consensus) %" PRIx64 " %s %s %s",
            msg_hash, block->dump().c_str(), network_self.to_string().c_str(), from_address.to_string().c_str());
        XMETRICS_GAUGE(metrics::xsync_recv_invalid_block, 1);
        return;
    }

    const std::string &address = block->get_account();
    if (!m_role_chains_mgr->exists(address)) {
        xsync_warn("xsync_handler receive push_newblock(no role) %" PRIx64 " %s %s", msg_hash, block->dump().c_str(), from_address.to_string().c_str());
        XMETRICS_GAUGE(metrics::xsync_recv_invalid_block, 1);
        return;
    }

    // to be deleted
    // check block existed already
    auto exist_block = m_sync_store->existed(block->get_account(), block->get_height(), block->get_viewid());
    if (exist_block) {
        XMETRICS_GAUGE(metrics::xsync_recv_duplicate_block, 1);
        return;
    }

    if (!check_auth(m_certauth, block)) {
        xsync_warn("xsync_handler receive push_newblock(auth failed) %" PRIx64 " %s %s", msg_hash, block->dump().c_str(), from_address.to_string().c_str());
        XMETRICS_GAUGE(metrics::xsync_recv_invalid_block, 1);
        return;
    }

    xsync_info("xsync_handler receive push_newblock %" PRIx64 " %s %s", msg_hash, block->dump().c_str(), from_address.to_string().c_str());

    mbus::xevent_ptr_t ev = make_object_ptr<mbus::xevent_blockfetcher_block_t>(block, network_self, from_address);
    m_block_fetcher->push_event(ev);
}

void xsync_handler_t::push_newblockhash(uint32_t msg_size,
        const vnetwork::xvnode_address_t &from_address,
        const vnetwork::xvnode_address_t &network_self,
        const xsync_message_header_ptr_t &header,
        base::xstream_t &stream,
        xtop_vnetwork_message::hash_result_type msg_hash,
        int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_new_hash, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_block_size, msg_size);

    auto ptr = make_object_ptr<xsync_message_general_newblockhash_t>();
    ptr->serialize_from(stream);

    const std::string &address = ptr->address;
    uint64_t height = ptr->height;
    const std::string &hash = ptr->hash;

    if (address == "" || height == 0 || hash == "")
        return;

    if (!common::has<common::xnode_type_t::storage>(network_self.type())) {
        xsync_warn("xsync_handler receive push_newblockhash(target must be archive) %" PRIx64 " %s,height=%lu, %s %s",
            msg_hash, address.c_str(), height, network_self.to_string().c_str(), from_address.to_string().c_str());
        return;
    }

    if (!common::has<common::xnode_type_t::rec>(from_address.type()) && !common::has<common::xnode_type_t::zec>(from_address.type()) &&
        !common::has<common::xnode_type_t::consensus>(from_address.type())) {
        xsync_warn("xsync_handler receive push_newblockhash(target must be consensus) %" PRIx64 " %s,height=%lu, %s %s",
            msg_hash, address.c_str(), height, network_self.to_string().c_str(), from_address.to_string().c_str());
        return;
    }

    xsync_dbg("xsync_handler receive push_newblockhash %" PRIx64 " wait(%ldms) %s,height=%lu %s,",
        msg_hash, get_time()-recv_time, address.c_str(), height, from_address.to_string().c_str());

    mbus::xevent_ptr_t ev = make_object_ptr<mbus::xevent_blockfetcher_blockhash_t>(address, height, hash, network_self, from_address);
    m_block_fetcher->push_event(ev);
}

void xsync_handler_t::broadcast_newblockhash(uint32_t msg_size,
        const vnetwork::xvnode_address_t &from_address,
        const vnetwork::xvnode_address_t &network_self,
        const xsync_message_header_ptr_t &header,
        base::xstream_t &stream,
        xtop_vnetwork_message::hash_result_type msg_hash,
        int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_broadcast_newblockhash, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_block_size, msg_size);

    auto ptr = make_object_ptr<xsync_message_general_newblockhash_t>();
    ptr->serialize_from(stream);

    const std::string &address = ptr->address;
    uint64_t height = ptr->height;
    const std::string &hash = ptr->hash;

    if (address == "" || height == 0 || hash == "")
        return;

    if (!common::has<common::xnode_type_t::storage_archive>(network_self.type())) {
        xsync_warn("xsync_handler receive broadcast_newblockhash(target must be archive) %" PRIx64 " %s,height=%lu, %s %s",
            msg_hash, address.c_str(), height, network_self.to_string().c_str(), from_address.to_string().c_str());
        return;
    }

    if (!common::has<common::xnode_type_t::storage_archive>(from_address.type())) {
        xsync_warn("xsync_handler receive broadcast_newblockhash(source must be archive) %" PRIx64 " %s,height=%lu, %s %s",
            msg_hash, address.c_str(), height, network_self.to_string().c_str(), from_address.to_string().c_str());
        return;
    }

    xsync_info("xsync_handler receive broadcast_newblockhash %" PRIx64 " wait(%ldms) %s,height=%lu %s,",
        msg_hash, get_time()-recv_time, address.c_str(), height, from_address.to_string().c_str());

    mbus::xevent_ptr_t ev = make_object_ptr<mbus::xevent_blockfetcher_blockhash_t>(address, height, hash, network_self, from_address);
    m_block_fetcher->push_event(ev);
}

void xsync_handler_t::blocks(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_blocks, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_blocks_size, msg_size);

    auto ptr = make_object_ptr<xsync_message_blocks_t>();
    ptr->serialize_from(stream);

    std::vector<data::xblock_ptr_t> &blocks = ptr->blocks;

    uint32_t count = blocks.size();

    if (count == 0) {
        xsync_info("xsync_handler receive blocks %" PRIx64 " wait(%ldms) count(%u) code(%u) %s",
            msg_hash, get_time()-recv_time, count, header->code, from_address.to_string().c_str());

        return;
    }

    XMETRICS_GAUGE(metrics::xsync_handler_blocks, count);

    xsync_info("xsync_handler receive blocks %" PRIx64 " wait(%ldms) %s count(%u) code(%u) %s",
        msg_hash, get_time()-recv_time, blocks[0]->get_account().c_str(), count, header->code, from_address.to_string().c_str());

    //move check to push_event
    mbus::xevent_ptr_t e = make_object_ptr<mbus::xevent_sync_response_blocks_t>(blocks, network_self, from_address);
    m_downloader->push_event(e);
}

void xsync_handler_t::gossip(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_gossip_recv, 1);
    XMETRICS_GAUGE(metrics::xsync_bytes_gossip_recv, msg_size);

    auto ptr = make_object_ptr<xsync_message_gossip_t>();
    ptr->serialize_from(stream);

    std::vector<xgossip_chain_info_ptr_t> &info_list = ptr->info_list;
    std::map<std::string, xgossip_behind_info_t> behind_chain_set;

    m_sync_gossip->handle_message(info_list, from_address, network_self, behind_chain_set);

    std::string reason = "gossip";

    for (auto &it: behind_chain_set) {
        const std::string &address = it.first;
        const xgossip_behind_info_t &info = it.second;
        if (info.local_height == info.peer_height)
            continue;

        mbus::xevent_ptr_t ev = make_object_ptr<mbus::xevent_behind_download_t>(address, 0u, info.peer_height, enum_chain_sync_policy_full, network_self, from_address, reason);
        m_downloader->push_event(ev);
    }
}

void xsync_handler_t::get_on_demand_blocks(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    if (m_sync_store->remove_empty_unit_forked()) {
        xwarn("xsync_handler_t::get_on_demand_blocks forked but recv old on demand request, drop it!");
        return;
    }
    XMETRICS_GAUGE(metrics::xsync_recv_get_on_demand_blocks, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_get_on_demand_blocks_bytes, msg_size);

    auto ptr = make_object_ptr<xsync_message_get_on_demand_blocks_t>();
    auto len = ptr->serialize_from(stream);
    if (len <= 0) {
        xerror("xsync_handler_t::get_on_demand_blocks deserialize fail");
        return;
    }

   m_sync_on_demand->handle_blocks_request(*(ptr.get()), from_address, network_self);
}

void xsync_handler_t::get_on_demand_blocks_with_proof(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {


    XMETRICS_GAUGE(metrics::xsync_recv_get_on_demand_blocks, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_get_on_demand_blocks_bytes, msg_size);

    auto ptr = make_object_ptr<xsync_message_get_on_demand_blocks_with_proof_t>();
    auto len = ptr->serialize_from(stream);
    if (len <= 0) {
        xerror("xsync_handler_t::get_on_demand_blocks_with_proof deserialize fail");
        return;
    }

   m_sync_on_demand->handle_blocks_request_with_proof(*(ptr.get()), from_address, network_self);
}

void xsync_handler_t::get_on_demand_blocks_with_hash(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_get_on_demand_blocks, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_get_on_demand_blocks_bytes, msg_size);

    auto ptr = make_object_ptr<xsync_message_get_on_demand_blocks_with_hash_t>();
    auto len = ptr->serialize_from(stream);
    if (len <= 0) {
        xerror("xsync_handler_t::get_on_demand_blocks_with_hash deserialize fail");
        return;
    }

   m_sync_on_demand->handle_blocks_request_with_hash(*(ptr.get()), from_address, network_self);
}

void xsync_handler_t::on_demand_blocks(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    if (m_sync_store->remove_empty_unit_forked()) {
        xwarn("xsync_handler_t::on_demand_blocks forked but recv old on demand response, drop it!");
        return;
    }

    XMETRICS_GAUGE(metrics::xsync_recv_on_demand_blocks, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_on_demand_blocks_bytes, msg_size);

    auto ptr = make_object_ptr<xsync_message_general_blocks_t>();
    auto len = ptr->serialize_from(stream);
    if (len <= 0) {
        xerror("xsync_handler_t::on_demand_blocks deserialize fail");
        return;
    }

    std::vector<xblock_ptr_t> &blocks = ptr->blocks;
    if (blocks.size() == 0)
        return;

    m_sync_on_demand->handle_blocks_response(blocks, from_address, network_self);
}

void xsync_handler_t::on_demand_blocks_with_proof(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_on_demand_blocks, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_on_demand_blocks_bytes, msg_size);

    auto ptr = make_object_ptr<xsync_message_general_blocks_with_proof_t>();
    auto len = ptr->serialize_from(stream);
    if (len <= 0) {
        xerror("xsync_handler_t::on_demand_blocks_with_proof deserialize fail");
        return;
    }

    std::vector<xblock_ptr_t> &blocks = ptr->blocks;
    if (blocks.size() == 0)
        return;
    std::string unit_proof_str = ptr->unit_proof_str;

    m_sync_on_demand->handle_blocks_response_with_proof(blocks, unit_proof_str, from_address, network_self);
}

void xsync_handler_t::on_demand_blocks_with_hash(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_on_demand_blocks, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_on_demand_blocks_bytes, msg_size);

    auto ptr = make_object_ptr<xsync_message_general_blocks_with_hash_t>();
    auto len = ptr->serialize_from(stream);
    if (len <= 0) {
        xerror("xsync_handler_t::on_demand_blocks_with_hash deserialize fail");
        return;
    }

    std::vector<xblock_ptr_t> &blocks = ptr->blocks;
    xdbg("xsync_handler_t::on_demand_blocks_with_hash blocks size:%u", blocks.size());
    if (blocks.size() == 0)
        return;

    m_sync_on_demand->handle_blocks_response_with_hash(blocks, from_address, network_self);
}

void xsync_handler_t::broadcast_chain_state(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_broadcast_chain_state, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_broadcast_chain_state_bytes, msg_size);

    auto ptr = make_object_ptr<xsync_message_chain_state_info_t>();
    ptr->serialize_from(stream);

    std::vector<xchain_state_info_t> &info_list = ptr->info_list;

#if 0
    // TODO if consensus node build peers with archives?
    if (network_self.type() != from_address.type())
        return;
#endif
    xsync_info("xsync_handler receive broadcast_chain_state %" PRIx64 " wait(%ldms) count:%u %s",
        msg_hash, get_time()-recv_time, (uint32_t)info_list.size(), from_address.to_string().c_str());

    if (info_list.size() > 500) {
        return;
    }

    // 1. update local peers
    m_peer_keeper->handle_message(network_self, from_address, info_list);

    std::shared_ptr<xrole_chains_t> role_chains = m_role_chains_mgr->get_role(network_self);
    if (role_chains == nullptr) {
        xsync_dbg("xsync_handler broadcast_chain_state network address %s is not exist", network_self.to_string().c_str());
        return;
    }

    const map_chain_info_t &chains = role_chains->get_chains_wrapper().get_chains();

    std::vector<xchain_state_info_t> rsp_info_list;
    // 2. response to peer
    for (auto &it: info_list) {

        const std::string &address = it.address;

        auto it2 = chains.find(address);
        if (it2 == chains.end()) {
            xsync_dbg("xsync_handler broadcast_chain_state chain address %s is not exist", address.c_str());
            continue;
        }

        const xchain_info_t &chain_info = it2->second;

        xchain_state_info_t info;
        info.address = address;
        if (chain_info.sync_policy == enum_chain_sync_policy_fast) {
            base::xauto_ptr<base::xvblock_t> latest_start_block = m_sync_store->get_latest_start_block(address, chain_info.sync_policy);
            xblock_ptr_t block = autoptr_to_blockptr(latest_start_block);
            if (!block->is_full_state_block()) {
                info.start_height = 0;
                info.end_height = 0;
            } else {
                info.start_height = latest_start_block->get_height();
                info.end_height = m_sync_store->get_latest_end_block_height(address, chain_info.sync_policy);
            }
        } else {
            info.start_height = m_sync_store->get_latest_start_block_height(address, chain_info.sync_policy);
            info.end_height = m_sync_store->get_latest_end_block_height(address, chain_info.sync_policy);
        }
        rsp_info_list.push_back(info);
    }

    if (rsp_info_list.size() != 0) {
        if (common::has<common::xnode_type_t::frozen>(network_self.type())) {
            m_sync_sender->send_frozen_response_chain_state(rsp_info_list, network_self, from_address);
        } else {
            m_sync_sender->send_response_chain_state(rsp_info_list, network_self, from_address);
        }
    }
}

void xsync_handler_t::response_chain_state(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_response_chain_state, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_response_chain_state_bytes, msg_size);

    auto ptr = make_object_ptr<xsync_message_chain_state_info_t>();
    ptr->serialize_from(stream);

    std::vector<xchain_state_info_t> &info_list = ptr->info_list;

    xsync_info("xsync_handler receive response_chain_state %" PRIx64 " wait(%ldms) count:%u %s",
        msg_hash, get_time()-recv_time, (uint32_t)info_list.size(), from_address.to_string().c_str());

    m_peer_keeper->handle_message(network_self, from_address, info_list);
}

void xsync_handler_t::cross_cluster_chain_state(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_cross_cluster_chain_state, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_cross_cluster_chain_state_bytes, msg_size);

    auto ptr = make_object_ptr<xsync_message_chain_state_info_t>();
    ptr->serialize_from(stream);
    std::vector<xchain_state_info_t> &info_list = ptr->info_list;

    if (!(common::has<common::xnode_type_t::storage_exchange>(network_self.type()) || (common::has<common::xnode_type_t::fullnode>(network_self.type())))) {
        xsync_warn("xsync_handler receive cross_cluster_chain_state(target must be archive or exchange) %" PRIx64 " count(%u), %s %s",
            msg_hash, info_list.size(), network_self.to_string().c_str(), from_address.to_string().c_str());
        return;
    }

    if (!common::has<common::xnode_type_t::rec>(from_address.type()) && !common::has<common::xnode_type_t::zec>(from_address.type()) &&
        !common::has<common::xnode_type_t::consensus>(from_address.type()) && !common::has<common::xnode_type_t::storage_archive>(from_address.type())) {
        xsync_warn("xsync_handler receive cross_cluster_chain_state(source must be consensus) %" PRIx64 " count(%u) %s %s",
            msg_hash, info_list.size(), network_self.to_string().c_str(), from_address.to_string().c_str());
        return;
    }

    xsync_info("xsync_handler receive cross_cluster_chain_state %" PRIx64 " wait(%ldms) count:%u %s",
        msg_hash, get_time()-recv_time, (uint32_t)info_list.size(), from_address.to_string().c_str());

    if (info_list.size() > 500) {
        return;
    }

    m_cross_cluster_chain_state->handle_message(network_self, from_address, info_list);
}

void xsync_handler_t::get_blocks_by_hashes(uint32_t msg_size,
        const vnetwork::xvnode_address_t &from_address,
        const vnetwork::xvnode_address_t &network_self,
        const xsync_message_header_ptr_t &header,
        base::xstream_t &stream,
        xtop_vnetwork_message::hash_result_type msg_hash,
        int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_get_blocks_by_hashes, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_get_blocks_by_hashes_bytes, msg_size);

    auto ptr = make_object_ptr<xsync_message_get_blocks_by_hashes_t>();
    ptr->serialize_from(stream);

    std::vector<xblock_hash_t> &info_list = ptr->info_list;

    if (info_list.size() == 0)
        return;

    xsync_info("xsync_handler receive get_blocks_by_hashes %" PRIx64 " wait(%ldms) count:%u %s",
        msg_hash, get_time()-recv_time, info_list.size(), from_address.to_string().c_str());

    std::vector<xblock_ptr_t> vector_blocks;

    for (auto &it: info_list) {
        xblock_hash_t &info = it;
        base::xauto_ptr<base::xvblock_t> vblock = m_sync_store->query_block(info.address, info.height, info.hash);
        if (vblock != nullptr) {
            xblock_ptr_t block_ptr = autoptr_to_blockptr(vblock);
            vector_blocks.push_back(block_ptr);
            xsync_info("xsync_handler receive get_blocks_by_hashes and success to query store, block hash is %s wait(%ldms), from address:%s",
                data::to_hex_str(info.hash).c_str(), get_time()-recv_time, from_address.to_string().c_str());
        } else {
            xsync_info("xsync_handler receive get_blocks_by_hashes and fail to query store, block hash is %s wait(%ldms), from address:%s",
                data::to_hex_str(info.hash).c_str(), get_time()-recv_time, from_address.to_string().c_str());
        }
    }

    if (!vector_blocks.empty()) {
        m_sync_sender->send_blocks_by_hashes(vector_blocks, network_self, from_address);
    }
}

void xsync_handler_t::blocks_by_hashes(uint32_t msg_size,
    const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_blocks_by_hashes, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_blocks_by_hashes_bytes, msg_size);

    auto ptr = make_object_ptr<xsync_message_general_blocks_t>();
    ptr->serialize_from(stream);

    std::vector<data::xblock_ptr_t> &blocks = ptr->blocks;

    uint32_t count = blocks.size();

    if (count == 0) {
        xsync_info("xsync_handler receive blocks_by_hashes %" PRIx64 " wait(%ldms) count(%u) code(%u) %s",
            msg_hash, get_time()-recv_time, count, header->code, from_address.to_string().c_str());

        return;
    }

    XMETRICS_GAUGE(metrics::xsync_handler_blocks_by_hashes, count);

    xsync_info("xsync_handler receive blocks_by_hashes %" PRIx64 " wait(%ldms) count(%u) code(%u) %s",
        msg_hash, get_time()-recv_time, count, header->code, from_address.to_string().c_str());

    mbus::xevent_ptr_t e = make_object_ptr<mbus::xevent_sync_response_blocks_t>(blocks, network_self, from_address);
    m_block_fetcher->push_event(e);
}

void xsync_handler_t::handle_role_change(const mbus::xevent_ptr_t& e) {
    if (e->minor_type == xevent_role_t::add_role) {
        auto bme = dynamic_xobject_ptr_cast<mbus::xevent_role_add_t>(e);
        std::shared_ptr<vnetwork::xvnetwork_driver_face_t> &vnetwork_driver = bme->m_vnetwork_driver;

        vnetwork::xvnode_address_t addr = vnetwork_driver->address();
        std::vector<uint16_t> table_ids = vnetwork_driver->table_ids();


        std::vector<common::xnode_address_t> parent_addresses;
        std::vector<common::xnode_address_t> neighbor_addresses;
        std::vector<common::xnode_address_t> archive_addresses;

        if (common::has<common::xnode_type_t::consensus_validator>(vnetwork_driver->type())) {
            auto const & parents_info = vnetwork_driver->parents_info2();
            for (auto const & info : parents_info) {
                parent_addresses.push_back(top::get<data::xnode_info_t>(info).address);
            }
        }

        // except edge
        if ((vnetwork_driver->type() & common::xnode_type_t::edge) == common::xnode_type_t::invalid) {
            auto const & neighbors_info = vnetwork_driver->neighbors_info2();
            for (auto const & info : neighbors_info) {
                neighbor_addresses.push_back(top::get<data::xnode_info_t>(info).address);
            }
        }

        std::set<uint16_t> set_table_ids;
        for (auto &id: table_ids)
            set_table_ids.insert(id);

        common::xminer_type_t miner_type = bme->m_miner_type;
        bool genesis = bme->m_genesis;
        m_role_xips_mgr->add_role(addr, neighbor_addresses, parent_addresses, vnetwork_driver, set_table_ids);
        m_role_xips_mgr->set_miner(miner_type, genesis);
        XMETRICS_GAUGE(metrics::xsync_cost_role_add_event, 1);
        int64_t tm1 = base::xtime_utl::gmttime_ms();

        std::string old_role_string = m_role_chains_mgr->get_roles_string();
        std::shared_ptr<xrole_chains_t> role_chains = std::make_shared<xrole_chains_t>(addr, set_table_ids);
        m_role_chains_mgr->add_role(role_chains);
        std::string new_role_string = m_role_chains_mgr->get_roles_string();

        int64_t tm2 = base::xtime_utl::gmttime_ms();
        xsync_kinfo("xsync_handler add_role_phase1 %s cost:%dms, %s", addr.to_string().c_str(), tm2-tm1, to_string(miner_type).c_str());

        xchains_wrapper_t& chains_wrapper = role_chains->get_chains_wrapper();
        const map_chain_info_t &chains = chains_wrapper.get_chains();
        if (genesis || common::has<common::xminer_type_t::archive>(miner_type) || common::has<common::xminer_type_t::exchange>(miner_type)) {
            // detect it is archive node
            if (store::enable_block_recycler(false))
                xinfo("disable_block_recycler ok.%s,%d", to_string(miner_type).c_str(), genesis);
            else
                xerror("disable_block_recycler fail.");
        } else if (common::xminer_type_t::invalid == miner_type) {
        } else {
            init_prune(chains, e);
            if (store::enable_block_recycler(true))
                xinfo("enable_block_recycler ok.");
            else
                xerror("enable_block_recycler fail.");
        }
        for (const auto & it : chains) {
            xevent_ptr_t ev = make_object_ptr<mbus::xevent_account_add_role_t>(it.second.address);
            m_downloader->push_event(ev);
            m_block_fetcher->push_event(ev);
        }

        m_sync_gossip->add_role(addr);
        m_peer_keeper->add_role(addr);

        int64_t tm3 = base::xtime_utl::gmttime_ms();
        xsync_kinfo("xsync_handler add_role_phase2 %s cost:%dms", addr.to_string().c_str(), tm3-tm2);
        xsync_kinfo("xsync_handler add_role_result(before) %s %s", addr.to_string().c_str(), old_role_string.c_str());
        xsync_kinfo("xsync_handler add_role_result(after) %s %s", addr.to_string().c_str(), new_role_string.c_str());

    } else if (e->minor_type == xevent_role_t::remove_role) {
        auto bme = dynamic_xobject_ptr_cast<mbus::xevent_role_remove_t>(e);
        std::shared_ptr<vnetwork::xvnetwork_driver_face_t> &vnetwork_driver = bme->m_vnetwork_driver;
        common::xminer_type_t miner_type = bme->m_miner_type;
        bool genesis = bme->m_genesis;

        vnetwork::xvnode_address_t addr = vnetwork_driver->address();
        std::vector<uint16_t> table_ids = vnetwork_driver->table_ids();
        std::set<uint16_t> set_table_ids;
        for (auto &id: table_ids)
            set_table_ids.insert(id);

        m_role_xips_mgr->remove_role(addr);

        XMETRICS_GAUGE(metrics::xsync_cost_role_remove_event, 1);

        int64_t tm1 = base::xtime_utl::gmttime_ms();

        std::string old_role_string = m_role_chains_mgr->get_roles_string();
        std::shared_ptr<xrole_chains_t> role_chains = std::make_shared<xrole_chains_t>(addr, set_table_ids);
        m_role_chains_mgr->remove_role(role_chains);
        std::string new_role_string = m_role_chains_mgr->get_roles_string();

        int64_t tm2 = base::xtime_utl::gmttime_ms();

        xsync_kinfo("xsync_handler remove_role_phase1 %s cost:%dms", addr.to_string().c_str(), tm2-tm1);

        xchains_wrapper_t& chains_wrapper = role_chains->get_chains_wrapper();
        const map_chain_info_t &chains = chains_wrapper.get_chains();
        for (const auto &it: chains) {
            if (common::has<common::xminer_type_t::advance>(miner_type) || common::has<common::xminer_type_t::validator>(miner_type)) {
                std::set<enum_height_type> types;
                if (common::has<common::xminer_type_t::validator>(miner_type)) {
                    types.insert(confirm_height);
                    xsync_kinfo("xsync_handler remove_role_phase1 del validator %s", it.second.address.c_str());
                }

                xsync_prune_sigleton_t::instance().del(it.second.address, types);
                if (xsync_prune_sigleton_t::instance().empty(it.second.address)){
                    base::xvaccount_t _vaddr(it.second.address);
                    xsync_kinfo("xsync_handler remove_role_phase1 unwatch sync %s", it.second.address.c_str());
                    store::unwatch_block_recycler(top::chainbase::xmodule_type_xsync, _vaddr);
                }
                if (common::has<common::xnode_type_t::consensus>(vnetwork_driver->type())) {
                    base::xvaccount_t _vaddr(it.second.address);
                    xsync_kinfo("xsync_handler remove_role_phase1 unwatch xtxpool %s", it.second.address.c_str());
                    store::unwatch_block_recycler(top::chainbase::xmodule_type_xtxpool, _vaddr);
                }
            }

            xevent_ptr_t ev = make_object_ptr<mbus::xevent_account_remove_role_t>(it.second.address);
            m_downloader->push_event(ev);
            m_block_fetcher->push_event(ev);
        }

        m_sync_gossip->remove_role(addr);
        m_peer_keeper->remove_role(addr);

        int64_t tm3 = base::xtime_utl::gmttime_ms();
        xsync_kinfo("xsync_handler remove_role_phase2 %s cost:%dms", addr.to_string().c_str(), tm3-tm2);
        xsync_kinfo("xsync_handler remove_role_result(before) %s %s", addr.to_string().c_str(), old_role_string.c_str());
        xsync_kinfo("xsync_handler remove_role_result(after) %s %s", addr.to_string().c_str(), new_role_string.c_str());
    }
}
int xsync_handler_t::init_prune(const map_chain_info_t &chains, const mbus::xevent_ptr_t& e) {
    auto bme = dynamic_xobject_ptr_cast<mbus::xevent_role_add_t>(e);
    std::shared_ptr<vnetwork::xvnetwork_driver_face_t> &vnetwork_driver = bme->m_vnetwork_driver;
    common::xminer_type_t miner_type = bme->m_miner_type;
    bool genesis = bme->m_genesis;

    for (const auto & it : chains) {
        if (common::has<common::xminer_type_t::advance>(miner_type) || common::has<common::xminer_type_t::validator>(miner_type)) {
            std::set<enum_height_type> types;
            if (common::has<common::xminer_type_t::advance>(miner_type)) {
                types.insert(mutable_checkpoint_height);
                base::xvaccount_t _vaddr(it.second.address);
                if (!_vaddr.is_drand_address()) {
                    types.insert(latest_state_height);
                }
                xsync_kinfo("xsync_handler add_role_phase1 add fullnode %s", it.second.address.c_str());
            } else {
                types.insert(confirm_height);
                xsync_kinfo("xsync_handler add_role_phase1 add validator %s", it.second.address.c_str());
            }
            xsync_prune_sigleton_t::instance().add(it.second.address, types);
            base::xvaccount_t _vaddr(it.second.address);
            store::watch_block_recycler(top::chainbase::xmodule_type_xsync, _vaddr);
            store::refresh_block_recycler_rule(top::chainbase::xmodule_type_xsync, _vaddr, 0);

            if (common::has<common::xnode_type_t::consensus>(vnetwork_driver->type())) {
                store::watch_block_recycler(top::chainbase::xmodule_type_xtxpool, _vaddr);
                store::refresh_block_recycler_rule(top::chainbase::xmodule_type_xtxpool, _vaddr, 0);
            }
        } 
    }
    for (uint32_t i = 0; i < MAIN_CHAIN_ZEC_TABLE_USED_NUM; i++) {
        std::string _vaddr = make_address_by_prefix_and_subaddr(sys_contract_zec_table_block_addr, uint16_t(i)).value();
        std::set<enum_height_type> types;
        types.insert(mutable_checkpoint_height);
        types.insert(latest_state_height);
        xsync_prune_sigleton_t::instance().add(_vaddr, types);

        store::watch_block_recycler(top::chainbase::xmodule_type_xsync, _vaddr);
        store::refresh_block_recycler_rule(top::chainbase::xmodule_type_xsync, _vaddr, 0);
        xsync_kinfo("xsync_handler add_role_phase1 add xsync %s", _vaddr.c_str());
        if (common::has<common::xnode_type_t::zec>(vnetwork_driver->type())) {
            store::watch_block_recycler(top::chainbase::xmodule_type_xtxpool, _vaddr);
            store::refresh_block_recycler_rule(top::chainbase::xmodule_type_xtxpool, _vaddr, 0);
            xsync_kinfo("xsync_handler add_role_phase1 add txpool %s", _vaddr.c_str());
        }
    }
    for (uint32_t i = 0; i < MAIN_CHAIN_REC_TABLE_USED_NUM; i++) {
        std::string _vaddr = make_address_by_prefix_and_subaddr(sys_contract_beacon_table_block_addr, uint16_t(i)).value();
        std::set<enum_height_type> types;
        types.insert(mutable_checkpoint_height);
        types.insert(latest_state_height);
        xsync_prune_sigleton_t::instance().add(_vaddr, types);

        store::watch_block_recycler(top::chainbase::xmodule_type_xsync, _vaddr);
        store::refresh_block_recycler_rule(top::chainbase::xmodule_type_xsync, _vaddr, 0);
        xsync_kinfo("xsync_handler add_role_phase1 add xsync %s", _vaddr.c_str());
        if (common::has<common::xnode_type_t::rec>(vnetwork_driver->type())) {
            store::watch_block_recycler(top::chainbase::xmodule_type_xtxpool, _vaddr);
            store::refresh_block_recycler_rule(top::chainbase::xmodule_type_xtxpool, _vaddr, 0);
            xsync_kinfo("xsync_handler add_role_phase1 add txpool %s", _vaddr.c_str());
        }
    }
    //}

    return 0;
}
void xsync_handler_t::handle_consensus_result(const mbus::xevent_ptr_t& e) {

    auto ptr = dynamic_xobject_ptr_cast<mbus::xevent_consensus_data_t>(e);
    xblock_ptr_t block = autoptr_to_blockptr(ptr->vblock_ptr);

    const std::string &address = block->get_account();
    if (!m_role_chains_mgr->exists(address)) {
        xsync_warn("xsync_handler on_consensus_block role not exist %s", block->get_account().c_str());
        return;
    }

    xsync_info("xsync_handler on_consensus_block %s", block->dump().c_str());

    m_sync_pusher->push_newblock_to_archive(block);
}

void xsync_handler_t::handle_chain_snapshot_request(
    uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    auto ptr = make_object_ptr<xsync_message_chain_snapshot_meta_t>();
    ptr->serialize_from(stream);

    XMETRICS_GAUGE(metrics::xsync_handle_chain_snapshot_request, 1);
    xsync_info("xsync_handler receive chain_snapshot_request %" PRIx64 " wait(%ldms) %s, account %s, height %llu",
        msg_hash, get_time()-recv_time, from_address.to_string().c_str(), ptr->m_account_addr.c_str(), ptr->m_height_of_fullblock);
    base::xauto_ptr<base::xvblock_t> blk = m_sync_store->load_block_object(ptr->m_account_addr, ptr->m_height_of_fullblock, false);
    if (blk != nullptr) {
        if (blk->get_block_level() == base::enum_xvblock_level_table && blk->get_block_class() == base::enum_xvblock_class_full) {
            // it must be full-table block now
            if (base::xvchain_t::instance().get_xstatestore()->get_blkstate_store()->get_full_block_offsnapshot(blk.get(), metrics::statestore_access_from_sync_chain_snapshot)) {
                std::string property_snapshot = blk->get_full_state();
                xsync_message_chain_snapshot_t chain_snapshot(ptr->m_account_addr,
                    property_snapshot, ptr->m_height_of_fullblock);
                m_sync_sender->send_chain_snapshot(chain_snapshot, xmessage_id_sync_chain_snapshot_response, network_self, from_address);
            } else {
                xsync_warn("xsync_handler receive chain_snapshot_request, and the full block state is not exist,account:%s, height:%llu, block_type:%d",
                    ptr->m_account_addr.c_str(), ptr->m_height_of_fullblock, blk->get_block_class());
            }
        } else {
            xsync_error("xsync_handler receive chain_snapshot_request, and it is not full table,account:%s, height:%llu",
                    ptr->m_account_addr.c_str(), ptr->m_height_of_fullblock);
        }
    } else {
        xsync_info("xsync_handler receive chain_snapshot_request, and the full block is not exist,account:%s, height:%llu",
                ptr->m_account_addr.c_str(), ptr->m_height_of_fullblock);
    }
}

void xsync_handler_t::handle_chain_snapshot_response(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    auto ptr = make_object_ptr<xsync_message_chain_snapshot_t>();
    ptr->serialize_from(stream);

    xsync_info("xsync_handler chain snapshot reponse %" PRIx64 " wait(%ldms) %s, account %s, height %llu",
        msg_hash, get_time()-recv_time, from_address.to_string().c_str(), ptr->m_tbl_account_addr.c_str(), ptr->m_height_of_fullblock);

    XMETRICS_GAUGE(metrics::xsync_handler_chain_snapshot_reponse, 1);

    mbus::xevent_ptr_t e = make_object_ptr<mbus::xevent_chain_snaphsot_t>(ptr->m_tbl_account_addr, ptr->m_chain_snapshot, ptr->m_height_of_fullblock, network_self, from_address);
    m_downloader->push_event(e);
}

void xsync_handler_t::handle_ondemand_chain_snapshot_request(
    uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    auto ptr = make_object_ptr<xsync_message_chain_snapshot_meta_t>();
    ptr->serialize_from(stream);

    XMETRICS_GAUGE(metrics::xsync_handle_ondemand_chain_snapshot_request, 1);
    xsync_info("xsync_handler receive ondemand_chain_snapshot_request %" PRIx64 " wait(%ldms) %s, account %s, height %llu",
        msg_hash, get_time()-recv_time, from_address.to_string().c_str(), ptr->m_account_addr.c_str(), ptr->m_height_of_fullblock);
    m_sync_on_demand->handle_chain_snapshot_meta(*(ptr.get()), network_self, from_address);
}

void xsync_handler_t::handle_ondemand_chain_snapshot_response(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    auto ptr = make_object_ptr<xsync_message_chain_snapshot_t>();
    ptr->serialize_from(stream);

    xsync_info("xsync_handler ondemand chain snapshot reponse %" PRIx64 " wait(%ldms) %s, account %s, height %llu",
        msg_hash, get_time()-recv_time, from_address.to_string().c_str(), ptr->m_tbl_account_addr.c_str(), ptr->m_height_of_fullblock);

    XMETRICS_GAUGE(metrics::xsync_handle_ondemand_chain_snapshot_reponse, 1);

    m_sync_on_demand->handle_chain_snapshot(*(ptr.get()), from_address, network_self);
}

void xsync_handler_t::get_on_demand_by_hash_blocks(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_on_demand_by_hash_blocks_req, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_on_demand_by_hash_blocks_req_bytes, msg_size);

    auto ptr = make_object_ptr<xsync_message_get_on_demand_by_hash_blocks_t>();
    ptr->serialize_from(stream);

   m_sync_on_demand->handle_blocks_by_hash_request(*(ptr.get()), from_address, network_self);
}

void xsync_handler_t::on_demand_by_hash_blocks(uint32_t msg_size, const vnetwork::xvnode_address_t &from_address,
    const vnetwork::xvnode_address_t &network_self,
    const xsync_message_header_ptr_t &header,
    base::xstream_t &stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_on_demand_by_hash_blocks_resp, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_on_demand_by_hash_blocks_resp_bytes, msg_size);

    auto ptr = make_object_ptr<xsync_message_general_blocks_t>();
    ptr->serialize_from(stream);

    std::vector<xblock_ptr_t> &blocks = ptr->blocks;
    if (blocks.size() == 0)
        return;

    m_sync_on_demand->handle_blocks_by_hash_response(blocks, from_address, network_self);
}
// val recv height from arc
void xsync_handler_t::recv_archive_height(uint32_t msg_size,
        const vnetwork::xvnode_address_t &from_address,
        const vnetwork::xvnode_address_t &network_self,
        const xsync_message_header_ptr_t &header,
        base::xstream_t &stream,
        xtop_vnetwork_message::hash_result_type msg_hash,
        int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_archive_height, 1);

    auto ptr = make_object_ptr<xchain_state_info_t>();
    ptr->serialize_from(stream);
    
    base::xvaccount_t _vaddr(ptr->address);
    if (ptr->end_height % 50 == 0) {
        xsync_prune_sigleton_t::instance().update(ptr->address, enum_height_type::confirm_height, ptr->end_height);
        xsync_info("refresh_block_recycler_rule succ: %s,%d", ptr->address.c_str(), ptr->end_height);
    }

    uint64_t latest_end_block_height = m_sync_store->get_latest_end_block_height(ptr->address, enum_chain_sync_policy_fast);
    xsync_dbg("recv_archive_height: %s, %llu, %llu", ptr->address.c_str(), ptr->end_height, latest_end_block_height);
    if (latest_end_block_height < ptr->end_height + 50)  // not send blocks within 50 blocks
        return;

    uint32_t count = 3;
    uint32_t start_height = ptr->end_height + 1;
    std::vector<xblock_ptr_t> vector_blocks;
    for (uint32_t height = start_height; height < start_height + count ; height++) {
        auto blocks = m_sync_store->load_block_objects(ptr->address, height);
        if (blocks.empty()) {
            break;
        }
        for (uint32_t j = 0; j < blocks.size(); j++){
            vector_blocks.push_back(xblock_t::raw_vblock_to_object_ptr(blocks[j].get()));
        }
    }
    xsync_info("recv_archive_height, send blocks: %s, %d, %d", ptr->address.c_str(), start_height, vector_blocks.size());
    XMETRICS_GAUGE(metrics::xsync_archive_height_blocks, vector_blocks.size());
    m_sync_sender->send_archive_blocks(xsync_msg_err_code_t::succ, ptr->address, vector_blocks, network_self, from_address);
}
// arc recv blocks from val
void xsync_handler_t::recv_archive_blocks(uint32_t msg_size,
        const vnetwork::xvnode_address_t &from_address,
        const vnetwork::xvnode_address_t &network_self,
        const xsync_message_header_ptr_t &header,
        base::xstream_t &stream,
        xtop_vnetwork_message::hash_result_type msg_hash,
        int64_t recv_time) {
    XMETRICS_GAUGE(metrics::xsync_recv_archive_blocks, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_archive_blocks_size, msg_size);

    auto ptr = make_object_ptr<xsync_message_blocks_t>();
    ptr->serialize_from(stream);

    std::vector<data::xblock_ptr_t> &blocks = ptr->blocks;

    uint32_t count = blocks.size();

    if (count == 0) {
        xsync_info("xsync_handler receive arc blocks %" PRIx64 " wait(%ldms) count(%u) code(%u) %s",
            msg_hash, get_time()-recv_time, count, header->code, from_address.to_string().c_str());

        return;
    }

    XMETRICS_GAUGE(metrics::xsync_handler_blocks, count);

    xsync_info("xsync_handler receive arc blocks %" PRIx64 " wait(%ldms) %s count(%u) code(%u) %s",
        msg_hash, get_time()-recv_time, blocks[0]->get_account().c_str(), count, header->code, from_address.to_string().c_str());

    if (false == sync_blocks_continue_check(blocks, "", false)) {
           xsync_warn("xsync_handler receive blocks(address error) (%s)",
                    blocks[0]->get_account().c_str());
        return ;
    }

    if (data::is_unit_address(common::xaccount_address_t{blocks[0]->get_account()}))
        return;

    mbus::xevent_ptr_t e = make_object_ptr<mbus::xevent_sync_archive_blocks_t>(blocks, network_self, from_address);
    m_downloader->push_event(e);        
}
// arc recv query from val on_timer
void xsync_handler_t::recv_query_archive_height(uint32_t msg_size,
        const vnetwork::xvnode_address_t &from_address,
        const vnetwork::xvnode_address_t &network_self,
        const xsync_message_header_ptr_t &header,
        base::xstream_t &stream,
        xtop_vnetwork_message::hash_result_type msg_hash,
        int64_t recv_time) {

    XMETRICS_GAUGE(metrics::xsync_recv_query_archive_height, 1);
    xsync_dbg("recv_query_archive_height.");

    // auto ptr = make_object_ptr<xsync_query_height_t>();
    auto ptr = make_object_ptr<xsync_message_chain_state_info_t>();
    ptr->serialize_from(stream);

    if (!(common::has<common::xnode_type_t::storage>(network_self.type()) || (common::has<common::xnode_type_t::fullnode>(network_self.type())))) {
        return;
    }

    std::vector<xchain_state_info_t> &info_list = ptr->info_list;
    xsync_info("xsync_handler recv_query_archive_height %" PRIx64 " wait(%ldms) count:%u %s",
        msg_hash, get_time()-recv_time, (uint32_t)info_list.size(), from_address.to_string().c_str());

    if (info_list.size() > 500) {
        return;
    }

    std::shared_ptr<xrole_chains_t> role_chains = m_role_chains_mgr->get_role(network_self);
    if (role_chains == nullptr) {
        xsync_dbg("xsync_handler recv_query_archive_height network address %s is not exist", network_self.to_string().c_str());
        return;
    }

    const map_chain_info_t &chains = role_chains->get_chains_wrapper().get_chains();
    std::vector<xchain_state_info_t> rsp_info_list;
    for (auto &it: info_list) {
        const std::string &address = it.address;
        auto it2 = chains.find(address);
        if (it2 == chains.end()) {
            xsync_dbg("xsync_handler recv_query_archive_height chain address %s not exist", address.c_str());
            continue;
        }
        xchain_state_info_t info;
        info.address = address;
        info.start_height = m_sync_store->get_latest_start_block_height(address, enum_chain_sync_policy_full);
        info.end_height = m_sync_store->get_latest_end_block_height(address, enum_chain_sync_policy_full);
        rsp_info_list.push_back(info);
/*    const std::shared_ptr<xrole_chains_t> &role_chains = m_role_chains_mgr->get_role(network_self);
    const map_chain_info_t & chains = role_chains->get_chains_wrapper().get_chains();
    std::vector<xchain_state_info_t> info_list;
    for (const auto & it : chains) {
        const std::string & address = it.first;
        const xchain_info_t & chain_info = it.second;

        xchain_state_info_t info;
        info.address = address;
        info.start_height = m_sync_store->get_latest_start_block_height(address, enum_chain_sync_policy_full);
        info.end_height = m_sync_store->get_latest_end_block_height(address, enum_chain_sync_policy_full);
        info_list.push_back(info);
        */
    }

    xsync_info("recv_query_archive_height, send height info %s count(%d)", network_self.to_string().c_str(), rsp_info_list.size());
    if (rsp_info_list.empty()) {
        return;
    }

    m_sync_sender->send_archive_height_list(rsp_info_list, network_self, from_address);
}
// val recv height list from arc
void xsync_handler_t::recv_archive_height_list(uint32_t msg_size,
        const vnetwork::xvnode_address_t &from_address,
        const vnetwork::xvnode_address_t &network_self,
        const xsync_message_header_ptr_t &header,
        base::xstream_t &stream,
        xtop_vnetwork_message::hash_result_type msg_hash,
        int64_t recv_time) {
    XMETRICS_GAUGE(metrics::xsync_recv_archive_height_list, 1);
    auto ptr = make_object_ptr<xsync_message_chain_state_info_t>();
    ptr->serialize_from(stream);

    std::vector<xchain_state_info_t> &info_list = ptr->info_list;
    xsync_info("xsync_handler receive recv_archive_height_list %" PRIx64 " wait(%ldms) count:%u %s",
        msg_hash, get_time()-recv_time, (uint32_t)info_list.size(), from_address.to_string().c_str());

    if (info_list.size() > 500) {
        return;
    }

    std::shared_ptr<xrole_chains_t> role_chains = m_role_chains_mgr->get_role(network_self);
    if (role_chains == nullptr) {
        xsync_dbg("xsync_handler recv_archive_height_list network address %s is not exist", network_self.to_string().c_str());
        return;
    }

    const map_chain_info_t &chains = role_chains->get_chains_wrapper().get_chains();
    std::vector<xchain_state_info_t> rsp_info_list;
    for (auto &it: info_list) {
        const std::string &address = it.address;
        auto it2 = chains.find(address);
        if (it2 == chains.end()) {
            xsync_dbg("xsync_handler recv_archive_height_list chain address %s not exist", address.c_str());
            continue;
        }

        uint64_t latest_end_block_height = m_sync_store->get_latest_end_block_height(address, enum_chain_sync_policy_fast);
        xsync_dbg("recv_archive_height_list: %s, %llu, %llu", address.c_str(), it.end_height, latest_end_block_height);
        if (latest_end_block_height < it.end_height + 50)  // not send blocks within 50 blocks
            continue;

        uint32_t count = 3;
        uint32_t start_height = it.end_height + 1;
        std::vector<xblock_ptr_t> vector_blocks;
        for (uint32_t height = start_height; height < start_height + count; height++) {
            auto blocks = m_sync_store->load_block_objects(address, height);
            if (blocks.empty()) {
                break;
            }
            for (uint32_t j = 0; j < blocks.size(); j++) {
                vector_blocks.push_back(xblock_t::raw_vblock_to_object_ptr(blocks[j].get()));
            }
            if (height % 50 == 0) {
                xsync_prune_sigleton_t::instance().update(address, enum_height_type::confirm_height, height);
                xsync_info("refresh_block_recycler_rule succ2: %s,%d", address.c_str(), height);
            }
        }
        xsync_info("recv_archive_height_list, send blocks: %d", vector_blocks.size());
        XMETRICS_GAUGE(metrics::xsync_archive_height_blocks, vector_blocks.size());
        m_sync_sender->send_archive_blocks(xsync_msg_err_code_t::succ, it.address, vector_blocks, network_self, from_address);
    }
}

int64_t xsync_handler_t::get_time() {
    return base::xtime_utl::gmttime_ms();
}

void xsync_handler_t::notify_deceit_node(const vnetwork::xvnode_address_t& address) {
    xsync_warn("xsync_handler deceit_node %s", address.to_string().c_str());
    m_blacklist->add_deceit_node(address);
    m_role_xips_mgr->remove_xips_by_id(address.node_id());
}

void xsync_handler_t::register_handler(xmessage_t::message_type msgid, xsync_handler_netmsg_callback cb) {
    m_handlers[msgid] = cb;
}

void xsync_handler_t::register_unused_handler(xmessage_t::message_type msgid) {
    m_unused_handlers[msgid] = 1;
}

bool xsync_handler_t::reject_hander_msg_after_block_forked(xmessage_t::message_type msg_type)
{
    if (m_sync_store->is_sync_protocal_forked()) {
        if (m_unused_handlers.find(msg_type) != m_unused_handlers.end()) {
            return false;
        }
    }
    return true;
}

void xsync_handler_t::on_block_request_process(uint32_t msg_size, const vnetwork::xvnode_address_t& from_address, 
                                            const vnetwork::xvnode_address_t& network_self, const xsync_message_header_ptr_t& header, 
                                            base::xstream_t& stream, xtop_vnetwork_message::hash_result_type msg_hash, int64_t recv_time)
{

    XMETRICS_GAUGE(metrics::xsync_getblocks_recv_req, 1);
    auto request_ptr = make_object_ptr<xsync_msg_block_request_t>();
    request_ptr->serialize_from(stream);

    if (!m_session_mgr->sync_block_request_valid_check(request_ptr)) {
        xsync_warn("xsync_handler::on_block_request_process request error %" PRIx64 "  self(%s)  from(%s). %s",
            msg_hash,  network_self.to_string().c_str(), from_address.to_string().c_str(), request_ptr->dump().c_str());
        return;
    }

    xsync_dbg("xsync_handler::on_block_request_process request  %" PRIx64 " self(%s) from(%s) %s ",
            msg_hash,  network_self.to_string().c_str(), from_address.to_string().c_str(), request_ptr->dump().c_str());

    if ((request_ptr->get_request_type() == enum_sync_block_request_demand)) {
        if (request_ptr->get_requeset_param_type() == enum_sync_block_by_height)  {
            handle_blocks_request_with_height(request_ptr, from_address, network_self);
        } else if (request_ptr->get_requeset_param_type() == enum_sync_block_by_hash) {
            handle_blocks_demand_request_with_hash(request_ptr, from_address, network_self);
        } else if (request_ptr->get_requeset_param_type() == enum_sync_block_by_txhash) {
            handle_blocks_request_with_txhash(request_ptr, from_address, network_self);
        }
    } else if (request_ptr->get_request_type() == enum_sync_block_request_ontime) {
        if (request_ptr->get_requeset_param_type() == enum_sync_block_by_height)  {
            handle_blocks_request_with_height(request_ptr, from_address, network_self);
        } else if (request_ptr->get_requeset_param_type() == enum_sync_block_by_hash) {
            handle_blocks_ontime_request_with_hash(request_ptr, from_address, network_self);
        } else if (request_ptr->get_requeset_param_type() == enum_sync_block_by_height_lists) {
            handle_blocks_ontime_request_with_height_lists(request_ptr, from_address, network_self);
        }
    }
}

void xsync_handler_t::on_block_response_process(uint32_t msg_size,
    const vnetwork::xvnode_address_t& from_address,
    const vnetwork::xvnode_address_t& network_self,
    const xsync_message_header_ptr_t& header,
    base::xstream_t& stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time)
{
    auto response_ptr = make_object_ptr<xsync_msg_block_response_t>();
    response_ptr->serialize_from(stream);

     xsync_msg_block_request_ptr_t reuqest_ptr = nullptr;
    if (false == m_session_mgr->sync_block_resopnse_valid_check(response_ptr, reuqest_ptr)) {
        xsync_warn("xsync_handler::on_block_response_process response not compare %" PRIx64 " %lx %s %s",
            msg_hash, response_ptr->get_sessionID(), network_self.to_string().c_str(), from_address.to_string().c_str());
        return;
    }

    xsync_dbg("xsync_handler_t:on_block_response_process receive  %" PRIx64 " wait(%ldms) session(%lx) type(%x) self(%s) from(%s)",
        msg_hash, get_time()-recv_time, response_ptr->get_sessionID(), response_ptr->get_request_type(), 
        network_self.to_string().c_str(), from_address.to_string().c_str());

    auto blocks_vec = response_ptr->get_all_xblock_ptr();

    switch (response_ptr->get_request_type()) {
    case enum_sync_block_request_demand: {
        if (response_ptr->get_requeset_param_type() == enum_sync_block_by_hash) {
            m_sync_on_demand->handle_blocks_response_with_hash(reuqest_ptr, blocks_vec, from_address, network_self);
        } else if (response_ptr->get_requeset_param_type() == enum_sync_block_by_height) {
            m_sync_on_demand->handle_blocks_response_with_params(blocks_vec, response_ptr->get_extend_data(), from_address, network_self);
        } else if (response_ptr->get_requeset_param_type() == enum_sync_block_by_txhash) {
            m_sync_on_demand->handle_blocks_by_hash_response(blocks_vec, from_address, network_self);
        }
    } break;
    case enum_sync_block_request_ontime: {
        if (response_ptr->get_requeset_param_type() == enum_sync_block_by_height) {
            mbus::xevent_ptr_t e = make_object_ptr<mbus::xevent_sync_response_blocks_t>(blocks_vec, network_self, from_address);
            m_downloader->push_event(e);
        } else if (response_ptr->get_requeset_param_type() == enum_sync_block_by_hash) {
            XMETRICS_GAUGE(metrics::xsync_handler_blocks_by_hashes, blocks_vec.size());
            mbus::xevent_ptr_t e = make_object_ptr<mbus::xevent_sync_response_blocks_t>(blocks_vec, network_self, from_address);
            m_block_fetcher->push_event(e);
        } else if (response_ptr->get_requeset_param_type() == enum_sync_block_by_height_lists) {
            mbus::xevent_ptr_t e = make_object_ptr<mbus::xevent_sync_archive_blocks_t>(blocks_vec, network_self, from_address);
            m_downloader->push_event(e);
        }
    } break;

    default:
        break;
    }
}

// new functions
void xsync_handler_t::on_block_push_newblock(uint32_t msg_size,
    const vnetwork::xvnode_address_t& from_address,
    const vnetwork::xvnode_address_t& network_self,
    const xsync_message_header_ptr_t& header,
    base::xstream_t& stream,
    xtop_vnetwork_message::hash_result_type msg_hash,
    int64_t recv_time)
{

    XMETRICS_GAUGE(metrics::xsync_recv_new_block, 1);
    XMETRICS_GAUGE(metrics::xsync_recv_block_size, msg_size);

    auto msg_push_ptr = make_object_ptr<xsync_msg_block_push_t>();
    msg_push_ptr->serialize_from(stream);

    xsync_dbg("xsync_handler_t:on_block_push_newblock receive  %" PRIx64 "  session(%lx) type(%x) self(%s) from(%s)",
        msg_hash,  msg_push_ptr->get_sessionID(), msg_push_ptr->get_request_type(), 
        network_self.to_string().c_str(), from_address.to_string().c_str());

    if (!m_session_mgr->sync_block_push_valid_check(msg_push_ptr)) {
        xsync_warn("xsync_handler::on_block_push_newblock sync_block_push_valid_check error. %" PRIx64 "  %s %s",
            msg_hash, network_self.to_string().c_str(), from_address.to_string().c_str());
        return;
    }

   auto blocks_vec = msg_push_ptr->get_all_xblock_ptr(); 
    if (blocks_vec.size() < 1 ) {
        xsync_warn("xsync_handler::on_block_push_newblock blocks_vec szie(%ld) : %" PRIx64 "  %s %s", blocks_vec.size(),
            msg_hash, network_self.to_string().c_str(), from_address.to_string().c_str());
        return;
    }

    xblock_ptr_t& block = blocks_vec[0];
    if (!(common::has<common::xnode_type_t::storage>(network_self.type()) || (common::has<common::xnode_type_t::fullnode>(network_self.type())))) {
        xsync_warn("xsync_handler_t::on_block_push_newblock  push_newblock(target must be archive or fullnode) %" PRIx64 " %s %s %s",
            msg_hash, block->dump().c_str(), network_self.to_string().c_str(), from_address.to_string().c_str());
        XMETRICS_GAUGE(metrics::xsync_recv_invalid_block, 1);
        return;
    }

    if (!common::has<common::xnode_type_t::rec>(from_address.type()) && !common::has<common::xnode_type_t::zec>(from_address.type()) && !common::has<common::xnode_type_t::consensus>(from_address.type()) && !common::has<common::xnode_type_t::evm>(from_address.type()) && !common::has<common::xnode_type_t::relay>(from_address.type())) {
        xsync_warn("xsync_handler_t::on_block_push_newblock  push_newblock(source must be consensus) %" PRIx64 " %s %s %s",
            msg_hash, block->dump().c_str(), network_self.to_string().c_str(), from_address.to_string().c_str());
        XMETRICS_GAUGE(metrics::xsync_recv_invalid_block, 1);
        return;
    }

    const std::string& address = block->get_account();
    if (!m_role_chains_mgr->exists(address)) {
        xsync_warn("xsync_handler_t::on_block_push_newblock receive push_newblock(no role) %" PRIx64 " %s %s", msg_hash, block->dump().c_str(), from_address.to_string().c_str());
        XMETRICS_GAUGE(metrics::xsync_recv_invalid_block, 1);
        return;
    }

    // to be deleted
    // check block existed already
    auto exist_block = m_sync_store->existed(block->get_account(), block->get_height(), block->get_viewid());
    if (exist_block) {
        XMETRICS_GAUGE(metrics::xsync_recv_duplicate_block, 1);
         xsync_warn("xsync_handler_t::on_block_push_newblock exist_block %" PRIx64 " %s %s", msg_hash, block->dump().c_str(), from_address.to_string().c_str());
        return;
    }

    if (!check_auth(m_certauth, block)) {
        xsync_warn("xsync_handler_t::on_block_push_newblock receive push_newblock(auth failed) %" PRIx64 " %s %s", msg_hash, block->dump().c_str(), from_address.to_string().c_str());
        XMETRICS_GAUGE(metrics::xsync_recv_invalid_block, 1);
        return;
    }

    xsync_info("xsync_handler_t::on_block_push_newblock push event %" PRIx64 " %s %s", msg_hash, block->dump().c_str(), from_address.to_string().c_str());

    mbus::xevent_ptr_t ev = make_object_ptr<mbus::xevent_blockfetcher_block_t>(block, network_self, from_address);
    m_block_fetcher->push_event(ev);
}

void xsync_handler_t::handle_blocks_request_with_height(const xsync_msg_block_request_ptr_t& request_ptr,
    const vnetwork::xvnode_address_t& to_address,
    const vnetwork::xvnode_address_t& network_self)
{
    std::string address = request_ptr->get_address();
    std::vector<xblock_ptr_t> vector_blocks;
    uint64_t start_height = request_ptr->get_request_start_height();
    uint64_t count = request_ptr->get_count();

    bool adjust_height = (((request_ptr->get_extend_bits() & enum_sync_block_height_adjust) > 0) ? true : false);

    if (count == 0) {
        return;
    }

    uint64_t end_height = start_height + count - 1;
    if (adjust_height) {
        base::xauto_ptr<base::xvblock_t> latest_full_block = m_sync_store->get_latest_full_block(address);
        if (latest_full_block != nullptr && latest_full_block->get_height() >= start_height && end_height >= latest_full_block->get_height()) {
            xblock_ptr_t block_ptr = autoptr_to_blockptr(latest_full_block);
            vector_blocks.push_back(block_ptr);
            start_height = latest_full_block->get_height() + 1;
        }
    }

    for (; (start_height <= end_height) && (vector_blocks.size() < max_request_block_count); start_height++) {
        auto need_blocks = m_sync_store->load_block_objects(address, start_height);
        if (need_blocks.empty()) {
            break;
        }
        for (uint32_t j = 0; j < need_blocks.size(); j++) {
            vector_blocks.push_back(xblock_t::raw_vblock_to_object_ptr(need_blocks[j].get()));
        }
    }

    if (vector_blocks.size() != 0) {
          xsync_info("xsync_handler_t::handle_blocks_request_with_height %s range[%llu,%llu]", address.c_str(),
              vector_blocks.front()->get_height(), vector_blocks.back()->get_height());
    }

    xsync_info("xsync_handler_t::handle_blocks_request_with_height response: %s ", request_ptr->dump().c_str());
    m_sync_sender->send_block_response(request_ptr, vector_blocks, 0, "", network_self, to_address);
}

void xsync_handler_t::handle_blocks_demand_request_with_hash(const xsync_msg_block_request_ptr_t& request_ptr,
    const vnetwork::xvnode_address_t& to_address,
    const vnetwork::xvnode_address_t& network_self)
{
    std::string address = request_ptr->get_address();
    std::vector<xblock_ptr_t> vector_blocks;
    uint64_t start_height = request_ptr->get_request_start_height();
    uint64_t heights = request_ptr->get_count();
    std::string last_unit_hash = request_ptr->get_requeset_param_str();

    bool adjust_height = (((request_ptr->get_extend_bits() & enum_sync_block_height_adjust) > 0) ? true : false);

    if (heights == 0 || last_unit_hash.empty()) {
        return;
    }

    xsync_info("xsync_handler_t::handle_blocks_demand_request_with_hash receive account %s, %s",
        address.c_str(), request_ptr->dump().c_str());

    uint64_t end_height = start_height + (uint64_t)heights - 1;
    xblock_ptr_t start_unit_block = nullptr;
    if (adjust_height) {
        base::xauto_ptr<base::xvblock_t> latest_full_block = m_sync_store->get_latest_full_block(address);
        if (latest_full_block != nullptr && latest_full_block->get_height() >= start_height && end_height >= latest_full_block->get_height()) {
            start_unit_block = autoptr_to_blockptr(latest_full_block);
            start_height = latest_full_block->get_height() + 1;
        }
    }

    std::string prev_hash = last_unit_hash;
    for (uint64_t height = end_height; height >= start_height; height--) {
        base::xvaccount_t account(address);
        auto block = m_sync_store->load_block_object(account, height, prev_hash);
        if (block == nullptr) {
            xwarn("xsync_handler_t::handle_blocks_demand_request_with_hash block load fail account:%s,height:%llu,hash:%s", address.c_str(), height, prev_hash.c_str());
            return;
        }
        xdbg("xsync_handler_t::handle_blocks_demand_request_with_hash add block:%s", block->dump().c_str());
        vector_blocks.insert(vector_blocks.begin(), autoptr_to_blockptr(block));
        prev_hash = block->get_last_block_hash();
    }
    if (start_unit_block != nullptr) {
        if (!vector_blocks.empty()) {
            if (start_unit_block->get_block_hash() != vector_blocks[0]->get_last_block_hash()) {
                xwarn("xsync_handler_t::handle_blocks_demand_request_with_hash hash not match");
                return;
            }
        }
        xdbg("xsync_handler_t::handle_blocks_demand_request_with_hash add full block:%s", start_unit_block->dump().c_str());
        vector_blocks.insert(vector_blocks.begin(), start_unit_block);
    }

    xsync_info("xsync_handler_t::handle_blocks_demand_request_with_hash response: %s ", request_ptr->dump().c_str());
    m_sync_sender->send_block_response(request_ptr, vector_blocks, 0, "", network_self, to_address);
}

void xsync_handler_t::handle_blocks_request_with_txhash(const xsync_msg_block_request_ptr_t& request,
    const vnetwork::xvnode_address_t& to_address,
    const vnetwork::xvnode_address_t& network_self)
{
    std::string address = request->get_address();
    std::string hash = request->get_requeset_param_str();

    xsync_info("xsync_handler_t::handle_blocks_request_with_txhash receive account %s, %s",
        address.c_str(), request->dump().c_str());

    std::vector<base::xvblock_ptr_t> blocks;
    base::xvchain_t::instance().get_xtxstore()->load_block_by_hash(hash, blocks);
    xdbg("handle_blocks_request_with_txhash, all:%d", blocks.size());
    if (blocks.empty())
        return;

    std::vector<data::xblock_ptr_t> vector_blocks;
    for (uint32_t i = 0; i < blocks.size(); i++) {
        if (blocks[i]->get_height() != 0) {
            xsync_dbg("xsync_handler_t::handle_blocks_request_with_txhash height not 0 owner %s hash:%s", blocks[i]->get_account().c_str(), data::to_hex_str(hash).c_str());
            vector_blocks.push_back(data::xblock_t::raw_vblock_to_object_ptr(blocks[i].get()));
        } else {
            xsync_dbg("xsync_handler_t::handle_blocks_request_with_txhash height is 0 owner %s hash:%s", blocks[i]->get_account().c_str(), data::to_hex_str(hash).c_str());
        }
    }

    xsync_info("xsync_handler_t::handle_blocks_request_with_txhash response account %s, %s block size(%ld).",address.c_str(), request->dump().c_str(), vector_blocks.size());
    m_sync_sender->send_block_response(request, vector_blocks, 0, "", network_self, to_address);
}

void xsync_handler_t::handle_blocks_ontime_request_with_hash(const xsync_msg_block_request_ptr_t &request, 
    const vnetwork::xvnode_address_t &to_address, const vnetwork::xvnode_address_t &network_self){

    XMETRICS_GAUGE(metrics::xsync_recv_get_blocks_by_hashes, 1);

    auto hash_vec_str =  request->get_requeset_param_str();
    base::xstream_t stream(base::xcontext_t::instance(), (uint8_t*)hash_vec_str.data(), hash_vec_str.size());
    uint32_t hash_count = 0;
    stream >> hash_count;
    std::vector<xblock_hash_t> info_list;
    for(uint32_t i=0; i<hash_count; i++){
         xblock_hash_t info;
        info.serialize_from(stream);
        info_list.push_back(info);
    }
    
    if (info_list.size() == 0)
        return;

    xsync_info("xsync_handler_t::handle_blocks_ontime_request_with_hash receive get_blocks_by_hashes count:%u %s %s .",
                info_list.size(), to_address.to_string().c_str(), request->dump().c_str());

    std::vector<xblock_ptr_t> vector_blocks;

    for (auto &it: info_list) {
        xblock_hash_t &info = it;
        base::xauto_ptr<base::xvblock_t> vblock = m_sync_store->query_block(info.address, info.height, info.hash);
        if (vblock != nullptr) {
            xblock_ptr_t block_ptr = autoptr_to_blockptr(vblock);
            vector_blocks.push_back(block_ptr);
            xsync_info("xsync_handler_t::handle_blocks_ontime_request_with_hash receive get_blocks_by_hashes and success to query store, block hash is %s from address:%s",
                data::to_hex_str(info.hash).c_str(), to_address.to_string().c_str());
        } else {
            xsync_info("xsync_handler_t::handle_blocks_ontime_request_with_hash receive get_blocks_by_hashes and fail to query store, block hash is %s  from address:%s",
                data::to_hex_str(info.hash).c_str(),  to_address.to_string().c_str());
        }
    }

    xsync_info("xsync_handler_t::handle_blocks_ontime_request_with_hash response  count:%u %s %s. blocks size(%ld) ",
                info_list.size(), to_address.to_string().c_str(), request->dump().c_str(), vector_blocks.size());
    m_sync_sender->send_block_response(request, vector_blocks, 0, "", network_self, to_address);

}

void xsync_handler_t::handle_blocks_ontime_request_with_height_lists(const xsync_msg_block_request_ptr_t &request, 
    const vnetwork::xvnode_address_t &to_address, const vnetwork::xvnode_address_t &network_self){

    XMETRICS_GAUGE(metrics::xsync_recv_archive_height_list, 1);

    auto info_list_str =  request->get_requeset_param_str();
    base::xstream_t stream(base::xcontext_t::instance(), (uint8_t*)info_list_str.data(), info_list_str.size());
    uint32_t list_count = 0;
    stream >> list_count;
    std::vector<xchain_state_info_t> info_list;
    for(uint32_t i=0; i<list_count; i++){
         xchain_state_info_t info;
        info.serialize_from(stream);
        info_list.push_back(info);
    }

    xsync_info("xsync_handler_t::handle_blocks_ontime_request_with_height_lists receive recv_archive_height_list count:%u %s",
         (uint32_t)info_list.size(), to_address.to_string().c_str());

    if (info_list.size() > 500) {
        return;
    }

    std::shared_ptr<xrole_chains_t> role_chains = m_role_chains_mgr->get_role(network_self);
    if (role_chains == nullptr) {
        xsync_dbg("xsync_handler_t::handle_blocks_ontime_request_with_height_lists recv_archive_height_list network address %s is not exist", network_self.to_string().c_str());
        return;
    }

    const map_chain_info_t &chains = role_chains->get_chains_wrapper().get_chains();
    std::vector<xchain_state_info_t> rsp_info_list;
    for (auto &it: info_list) {
        const std::string &address = it.address;
        auto it2 = chains.find(address);
        if (it2 == chains.end()) {
            xsync_dbg("xsync_handler_t::handle_blocks_ontime_request_with_height_lists recv_archive_height_list chain address %s not exist", address.c_str());
            continue;
        }

        uint64_t latest_end_block_height = m_sync_store->get_latest_end_block_height(address, enum_chain_sync_policy_fast);
        xsync_dbg("recv_archive_height_list: %s, %llu, %llu", address.c_str(), it.end_height, latest_end_block_height);
        if (latest_end_block_height < it.end_height + 50)  // not send blocks within 50 blocks
            continue;

        uint32_t count = 3;
        uint32_t start_height = it.end_height + 1;
        std::vector<xblock_ptr_t> vector_blocks;
        for (uint32_t height = start_height; height < start_height + count; height++) {
            auto blocks = m_sync_store->load_block_objects(address, height);
            if (blocks.empty()) {
                break;
            }
            for (uint32_t j = 0; j < blocks.size(); j++) {
                vector_blocks.push_back(xblock_t::raw_vblock_to_object_ptr(blocks[j].get()));
            }
            if (height % 50 == 0) {
                xsync_prune_sigleton_t::instance().update(address, enum_height_type::confirm_height, height);
                xsync_info("xsync_handler_t::handle_blocks_ontime_request_with_height_lists succ2: %s,%d", address.c_str(), height);
            }
        }
        xsync_info("xsync_handler_t::handle_blocks_ontime_request_with_height_lists, send blocks: %d", vector_blocks.size());
        XMETRICS_GAUGE(metrics::xsync_archive_height_blocks, vector_blocks.size());
        // request one, but response much
        m_sync_sender->send_block_response(request, vector_blocks, 0, "", network_self, to_address);
    }
}

NS_END2
