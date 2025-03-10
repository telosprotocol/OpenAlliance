#include "gtest/gtest.h"

#define private public
#define protected public

#include "test_common.hpp"
#include "xblockmaker/xtable_maker.h"
#include "xchain_fork/xchain_upgrade_center.h"
#include "tests/mock/xvchain_creator.hpp"
#include "tests/mock/xdatamock_table.hpp"
#include "tests/mock/xdatamock_address.hpp"
#include "xdata/xnative_contract_address.h"
#include "xblockmaker/xproposal_maker.h"

using namespace top;
using namespace top::base;
using namespace top::mbus;
using namespace top::store;
using namespace top::data;
using namespace top::mock;
using namespace top::blockmaker;

class test_tablemaker : public testing::Test {
protected:
    void SetUp() override {
        chain_fork::xtop_chain_fork_config_center::init();
        base::xvblock_fork_t::instance().init(chain_fork::xtop_chain_fork_config_center::is_block_forked);
    }

    void TearDown() override {
    }
};

TEST_F(test_tablemaker, make_proposal_1) {
    xblockmaker_resources_ptr_t resources = std::make_shared<test_xblockmaker_resources_t>();

    mock::xdatamock_table mocktable(1, 2);
    std::string table_addr = mocktable.get_account();
    std::vector<std::string> unit_addrs = mocktable.get_unit_accounts();
    std::string from_addr = unit_addrs[0];
    std::string to_addr = unit_addrs[1];

    mocktable.store_genesis_units(resources->get_blockstore());

    std::vector<xcons_transaction_ptr_t> send_txs = mocktable.create_send_txs(from_addr, to_addr, 2);
    xtable_maker_ptr_t tablemaker = make_object_ptr<xtable_maker_t>(table_addr, resources);

    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        table_para.set_origin_txs(send_txs);
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 1);

        xtablemaker_para_t table_para2(mocktable.get_table_state(), mocktable.get_commit_table_state());
        table_para2.set_origin_txs(send_txs);
        int32_t ret = tablemaker->verify_proposal(proposal_block.get(), table_para2, proposal_para);
        xassert(ret == 0);

        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    }
    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 2);

        xtablemaker_para_t table_para2(mocktable.get_table_state(), mocktable.get_commit_table_state());
        table_para2.set_other_accounts(table_para.get_proposal()->get_other_accounts());
        int32_t ret = tablemaker->verify_proposal(proposal_block.get(), table_para2, proposal_para);
        xassert(ret == 0);

        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    }
    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 3);

        xtablemaker_para_t table_para2(mocktable.get_table_state(), mocktable.get_commit_table_state());
        table_para2.set_other_accounts(table_para.get_proposal()->get_other_accounts());
        int32_t ret = tablemaker->verify_proposal(proposal_block.get(), table_para2, proposal_para);
        xassert(ret == 0);

        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    }
    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block == nullptr);        
    }    
}

TEST_F(test_tablemaker, select_peer_sids_for_confirm_id) {
    std::vector<base::xtable_shortid_t> all_sid_vec;
    for (uint16_t i = 0; i < enum_vbucket_has_tables_count; i++) {
        base::xtable_index_t tableindex(base::enum_chain_zone_consensus_index, i);
        all_sid_vec.push_back(tableindex.to_table_shortid());
    }
    for (uint16_t i = 0; i < MAIN_CHAIN_REC_TABLE_USED_NUM; i++) {
        base::xtable_index_t tableindex(base::enum_chain_zone_beacon_index, i);
        all_sid_vec.push_back(tableindex.to_table_shortid());
    }
    for (uint16_t i = 0; i < MAIN_CHAIN_ZEC_TABLE_USED_NUM; i++) {
        base::xtable_index_t tableindex(base::enum_chain_zone_zec_index, i);
        all_sid_vec.push_back(tableindex.to_table_shortid());
    }

    // for (uint32_t height = 0; height < 1000; height ++) {
    //     auto select_sids = xproposal_maker_t::select_peer_sids_for_confirm_id(all_sid_vec, height);
    //     assert(select_sids.size() == 1);
    //     std::vector<base::xtable_shortid_t> select_sids_vec;
    //     select_sids_vec.assign(select_sids.begin(), select_sids.end());
    //     assert(select_sids_vec[0] == all_sid_vec[height%all_sid_vec.size()]);
    // }

    uint64_t pos = 0;
    uint64_t mulriple = 0;
    uint64_t height = 0;

    uint32_t batch_num = 4;
    uint64_t height_interval = 4;

    for (pos = 1; pos < height_interval; pos++) {
        for (mulriple = 0; mulriple < 100; mulriple++) {
            height = pos + mulriple*height_interval;
            auto select_sids = xproposal_maker_t::select_peer_sids_for_confirm_id(all_sid_vec, height);
            ASSERT_EQ(select_sids.empty(), true);
        }
    }

    uint64_t times_to_traverse_all_sids = (all_sid_vec.size() + batch_num - 1)/batch_num;
    uint64_t traverse_times = 0;

    for (mulriple = 0; mulriple < 100; mulriple++) {
        height = mulriple*height_interval;
        auto select_sids = xproposal_maker_t::select_peer_sids_for_confirm_id(all_sid_vec, height);
        std::vector<base::xtable_shortid_t> select_sids_vec;
        select_sids_vec.assign(select_sids.begin(), select_sids.end());

        assert(select_sids_vec[0] == all_sid_vec[0 + traverse_times*batch_num]);
        assert(select_sids_vec[1] == all_sid_vec[1 + traverse_times*batch_num]);
        assert(select_sids_vec[2] == all_sid_vec[2 + traverse_times*batch_num]);
        assert(select_sids_vec[3] == all_sid_vec[3 + traverse_times*batch_num]);

        if (traverse_times == times_to_traverse_all_sids - 1) {
            traverse_times = 0;
        } else {
            traverse_times++;
        }
    }
    

    // uint64_t pos = 0;
    // uint64_t mulriple = 0;
    // uint64_t height = 0;

    // uint32_t batch_num = 16;
    // uint64_t height_interval = 8;

    // for (pos = 1; pos < height_interval; pos++) {
    //     for (mulriple = 0; mulriple < 100; mulriple++) {
    //         height = pos + mulriple*height_interval;
    //         auto select_sids = xproposal_maker_t::select_peer_sids_for_confirm_id(all_sid_vec, height);
    //         ASSERT_EQ(select_sids.empty(), true);
    //     }
    // }

    // uint64_t times_to_traverse_all_sids = (all_sid_vec.size() + batch_num - 1)/batch_num;
    // uint64_t traverse_times = 0;

    // for (mulriple = 0; mulriple < 100; mulriple++) {
    //     height = mulriple*height_interval;
    //     auto select_sids = xproposal_maker_t::select_peer_sids_for_confirm_id(all_sid_vec, height);
    //     std::vector<base::xtable_shortid_t> select_sids_vec;
    //     select_sids_vec.assign(select_sids.begin(), select_sids.end());

    //     assert(select_sids_vec[0] == all_sid_vec[0 + traverse_times*batch_num]);
    //     assert(select_sids_vec[1] == all_sid_vec[1 + traverse_times*batch_num]);
    //     assert(select_sids_vec[2] == all_sid_vec[2 + traverse_times*batch_num]);
    //     assert(select_sids_vec[3] == all_sid_vec[3 + traverse_times*batch_num]);
    //     if (traverse_times < times_to_traverse_all_sids - 1) {
    //         assert(select_sids_vec[4] == all_sid_vec[4 + traverse_times*batch_num]);
    //         assert(select_sids_vec[5] == all_sid_vec[5 + traverse_times*batch_num]);
    //         assert(select_sids_vec[6] == all_sid_vec[6 + traverse_times*batch_num]);
    //         assert(select_sids_vec[7] == all_sid_vec[7 + traverse_times*batch_num]);
    //         assert(select_sids_vec[8] == all_sid_vec[8 + traverse_times*batch_num]);
    //         assert(select_sids_vec[9] == all_sid_vec[9 + traverse_times*batch_num]);
    //         assert(select_sids_vec[10] == all_sid_vec[10 + traverse_times*batch_num]);
    //         assert(select_sids_vec[11] == all_sid_vec[11 + traverse_times*batch_num]);
    //         assert(select_sids_vec[12] == all_sid_vec[12 + traverse_times*batch_num]);
    //         assert(select_sids_vec[13] == all_sid_vec[13 + traverse_times*batch_num]);
    //         assert(select_sids_vec[14] == all_sid_vec[14 + traverse_times*batch_num]);
    //         assert(select_sids_vec[15] == all_sid_vec[15 + traverse_times*batch_num]);
    //     }

    //     if (traverse_times == times_to_traverse_all_sids - 1) {
    //         traverse_times = 0;
    //     } else {
    //         traverse_times++;
    //     }
    // }
}

TEST_F(test_tablemaker, make_proposal_block_build_hash_count) {
#ifdef ENABLE_METRICS
    xblockmaker_resources_ptr_t resources = std::make_shared<test_xblockmaker_resources_t>();

    mock::xdatamock_table mocktable(1, 100);
    std::string table_addr = mocktable.get_account();
    std::vector<std::string> unit_addrs = mocktable.get_unit_accounts();
    std::string from_addr = unit_addrs[0];
    std::string to_addr = unit_addrs[1];

    resources->get_txpool()->subscribe_tables(0, 1, 1,common::xnode_type_t::consensus_auditor);

    mocktable.store_genesis_units(resources->get_blockstore());

    std::vector<xcons_transaction_ptr_t> send_txs = mocktable.create_send_txs(from_addr, to_addr, 100);
    xtable_maker_ptr_t tablemaker = make_object_ptr<xtable_maker_t>(table_addr, resources);

    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        table_para.set_origin_txs(send_txs);
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        int64_t cur_hash_count =  XMETRICS_GAUGE_GET_VALUE(metrics::cpu_hash_256_calc);
        //std::cout << "before cur_hash_count " << cur_hash_count << std::endl;

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 1);

        int64_t last_hash_count =  XMETRICS_GAUGE_GET_VALUE(metrics::cpu_hash_256_calc);
        //std::cout << "after last_hash_count, " << last_hash_count << " count " << (last_hash_count - cur_hash_count)<< std::endl;

    }
#endif
}


TEST_F(test_tablemaker, make_proposal_verify_build_hash_count) {
#ifdef ENABLE_METRICS
    xblockmaker_resources_ptr_t resources = std::make_shared<test_xblockmaker_resources_t>();

    mock::xdatamock_table mocktable(1, 100);
    std::string table_addr = mocktable.get_account();
    std::vector<std::string> unit_addrs = mocktable.get_unit_accounts();
    std::string from_addr = unit_addrs[0];
    std::string to_addr = unit_addrs[1];

    mocktable.store_genesis_units(resources->get_blockstore());

    int64_t cur_hash_count =  XMETRICS_GAUGE_GET_VALUE(metrics::cpu_hash_256_calc);
    //std::cout << "before cur_hash_count " << cur_hash_count << std::endl;

    std::vector<xcons_transaction_ptr_t> send_txs = mocktable.create_send_txs(from_addr, to_addr, 4);
    xtable_maker_ptr_t tablemaker = make_object_ptr<xtable_maker_t>(table_addr, resources);

    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        table_para.set_origin_txs(send_txs);
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 1);
        
        int64_t table_make_hash_count =  XMETRICS_GAUGE_GET_VALUE(metrics::cpu_hash_256_calc);
        /* std::cout << "after table make_proposal table_make_hash_count, " << table_make_hash_count << " count " \
         << (table_make_hash_count - cur_hash_count)<< std::endl;*/
      
        xtablemaker_para_t table_para2(mocktable.get_table_state(), mocktable.get_commit_table_state());
        table_para2.set_origin_txs(send_txs);
        int32_t ret = tablemaker->verify_proposal(proposal_block.get(), table_para2, proposal_para);
        xassert(ret == 0);

        int64_t verify_make_hash_count =  XMETRICS_GAUGE_GET_VALUE(metrics::cpu_hash_256_calc);
       /* std::cout << "after table verify_proposal verify_make_hash_count, " << verify_make_hash_count << " count " \
        << (verify_make_hash_count - table_make_hash_count)<< std::endl;*/
          
       int64_t before_unpack_count =  XMETRICS_GAUGE_GET_VALUE(metrics::cpu_hash_256_calc);
        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
     
        int64_t last_unpack_count =  XMETRICS_GAUGE_GET_VALUE(metrics::cpu_hash_256_calc);
        //std::cout << "after last_unpack_count, " << last_unpack_count << " count " << (last_unpack_count - before_unpack_count)<< std::endl;


    }
#endif
}


TEST_F(test_tablemaker, make_unpack_units_hash_8_4_count) {
#ifdef ENABLE_METRICS
    xblockmaker_resources_ptr_t resources = std::make_shared<test_xblockmaker_resources_t>();

    mock::xdatamock_table mocktable(1, 100);
    std::string table_addr = mocktable.get_account();
    std::vector<std::string> unit_addrs = mocktable.get_unit_accounts();

    mocktable.store_genesis_units(resources->get_blockstore());

    int64_t cur_hash_count =  XMETRICS_GAUGE_GET_VALUE(metrics::cpu_hash_256_calc);
    //std::cout << "before cur_hash_count " << cur_hash_count << std::endl;

    int account_count = 4;
    int transaction_count = 100;
    std::vector<xcons_transaction_ptr_t> all_txs;
    std::string to_addr = unit_addrs[0];
    for (int i = 1; i < account_count; i++)
    {
       std::string from_addr = unit_addrs[i];
       std::vector<xcons_transaction_ptr_t> send_txs = mocktable.create_send_txs(from_addr, to_addr, transaction_count);
       all_txs.insert(all_txs.end(),send_txs.begin(),send_txs.end());
    }
    //std::cout << "account count "<< account_count << ", and every send count " << transaction_count << std::endl;
    xtable_maker_ptr_t tablemaker = make_object_ptr<xtable_maker_t>(table_addr, resources);
    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        table_para.set_origin_txs(all_txs);
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 1);
        
        int64_t table_make_hash_count =  XMETRICS_GAUGE_GET_VALUE(metrics::cpu_hash_256_calc);
        /*std::cout << "after table make_proposal table_make_hash_count, " << table_make_hash_count << " count " \
         << (table_make_hash_count - cur_hash_count)<< std::endl;*/
      
        xtablemaker_para_t table_para2(mocktable.get_table_state(), mocktable.get_commit_table_state());
        table_para2.set_origin_txs(all_txs);
        int32_t ret = tablemaker->verify_proposal(proposal_block.get(), table_para2, proposal_para);
        xassert(ret == 0);

        int64_t verify_make_hash_count =  XMETRICS_GAUGE_GET_VALUE(metrics::cpu_hash_256_calc);
        /*std::cout << "after table verify_proposal verify_make_hash_count, " << verify_make_hash_count << " count " \
        << (verify_make_hash_count - table_make_hash_count)<< std::endl;*/
          
        int64_t before_unpack_count =  XMETRICS_GAUGE_GET_VALUE(metrics::cpu_hash_256_calc);
        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
     
        int64_t last_unpack_count =  XMETRICS_GAUGE_GET_VALUE(metrics::cpu_hash_256_calc);
         xassert( (last_unpack_count - before_unpack_count) <= 638);
        //std::cout << "after last_unpack_count, " << last_unpack_count << " count " << (last_unpack_count - before_unpack_count)<< std::endl;
    }
#endif
}


TEST_F(test_tablemaker, make_receipt_hash_count) {
#ifdef ENABLE_METRICS
    xblockmaker_resources_ptr_t resources = std::make_shared<test_xblockmaker_resources_t>();

    mock::xdatamock_table mocktable(1, 100);
    std::string table_addr = mocktable.get_account();
    std::vector<std::string> unit_addrs = mocktable.get_unit_accounts();

    mocktable.store_genesis_units(resources->get_blockstore());
    
    xtable_maker_ptr_t tablemaker = make_object_ptr<xtable_maker_t>(table_addr, resources);

    {
         int account_count = 64;
        int transaction_count = 4;
        std::vector<xcons_transaction_ptr_t> all_txs;
        std::string to_addr = mock::xdatamock_address::make_unit_address(base::enum_chain_zone_consensus_index, 9);
        for (int i = 1; i <= account_count; i++)
        {
            std::string from_addr = unit_addrs[i];
            std::vector<xcons_transaction_ptr_t> send_txs = mocktable.create_send_txs(from_addr, to_addr, transaction_count);
            all_txs.insert(all_txs.end(),send_txs.begin(),send_txs.end());
        }

         std::cout << "account count "<< account_count << ", and every send count " << transaction_count << std::endl;
          xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        table_para.set_origin_txs(all_txs);
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 1);

        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    }

    for(int block_index = 2 ; block_index < 4; block_index++){

            xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
            xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

            xtablemaker_result_t table_result;
            xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
            xassert(proposal_block != nullptr);
           // xassert(proposal_block->get_height() == block_index);

            mocktable.do_multi_sign(proposal_block);
            mocktable.on_table_finish(proposal_block);
            resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    }

    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        auto tableblocks = mocktable.get_history_tables();

        int64_t before_receipt_hash_count =  XMETRICS_GAUGE_GET_VALUE(metrics::cpu_hash_256_calc);
        std::cout << "before_receipt_hash_count " << before_receipt_hash_count << std::endl;
        xinfo("before_receipt_hash_count %d ",before_receipt_hash_count);
        std::vector<xcons_transaction_ptr_t> recv_txs = mocktable.create_receipts(tableblocks[1]);
        int64_t last_receipt_hash_count =  XMETRICS_GAUGE_GET_VALUE(metrics::cpu_hash_256_calc);
        std::cout << "after last_receipt_hash_count  " << last_receipt_hash_count << ", count "\
         << (last_receipt_hash_count - before_receipt_hash_count) << std::endl;
 
      //  xassert(recv_txs.size() == 2);
        std::cout << "recv_txs[0]->get_last_action_receipt_id() " << recv_txs[0]->get_last_action_receipt_id() << std::endl;
        std::cout << "recv_txs[0]->get_last_action_sender_confirmed_receipt_id() " << recv_txs[0]->get_last_action_sender_confirmed_receipt_id() << std::endl;
        std::cout << "recv_txs[1]->get_last_action_receipt_id() " << recv_txs[1]->get_last_action_receipt_id() << std::endl;
        std::cout << "recv_txs[1]->get_last_action_sender_confirmed_receipt_id() " << recv_txs[1]->get_last_action_sender_confirmed_receipt_id() << std::endl;    
       // xassert(recv_txs[0]->get_last_action_receipt_id() == 1);
       // xassert(recv_txs[1]->get_last_action_receipt_id() == 2);

    // {
    //     mock::xdatamock_table mocktable_recv(9, 4);
    //     std::string table_addr_recv = mocktable_recv.get_account();
    //     xtablemaker_para_t table_para_recv(mocktable_recv.get_table_state(), mocktable_recv.get_commit_table_state());
    //     table_para_recv.set_origin_txs(recv_txs);
    //     xblock_consensus_para_t proposal_para_recv = mocktable_recv.init_consensus_para();
    //     xtablemaker_result_t table_result_recv;
    //     xblock_ptr_t proposal_block_recv = tablemaker->make_proposal(table_para_recv, proposal_para_recv, table_result_recv);
    //     xassert(proposal_block_recv != nullptr);
    //     xassert(proposal_block_recv->get_height() == 1);

    //     mocktable_recv.do_multi_sign(proposal_block_recv);
    //     mocktable_recv.on_table_finish(proposal_block_recv);
    //     resources->get_blockstore()->store_block(mocktable_recv, proposal_block_recv.get());        
    // }
    }
#endif
}


TEST_F(test_tablemaker, make_receipt_hash_new_count) {
#ifdef ENABLE_METRICS  
    xblockmaker_resources_ptr_t resources = std::make_shared<test_xblockmaker_resources_t>();

    mock::xdatamock_table mocktable(1, 100);
    std::string table_addr = mocktable.get_account();
    std::vector<std::string> unit_addrs = mocktable.get_unit_accounts();
    std::string from_addr = unit_addrs[0];
    std::string to_addr = unit_addrs[1];

    mocktable.store_genesis_units(resources->get_blockstore());
    
    xtable_maker_ptr_t tablemaker = make_object_ptr<xtable_maker_t>(table_addr, resources);

    {
         int account_count = 64;
        int transaction_count = 4;
        std::vector<xcons_transaction_ptr_t> all_txs;
        std::string to_addr = unit_addrs[0];
        for (int i = 1; i <= account_count; i++)
        {
            std::string from_addr = unit_addrs[i];
            std::vector<xcons_transaction_ptr_t> send_txs = mocktable.create_send_txs(from_addr, to_addr, transaction_count);
            all_txs.insert(all_txs.end(),send_txs.begin(),send_txs.end());
        }

        // std::cout << "account count "<< account_count << ", and every send count " << transaction_count << std::endl;
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        table_para.set_origin_txs(all_txs);
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 1);

        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    }

    for(int block_index = 2 ; block_index < 4; block_index++){

            xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
            xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

            xtablemaker_result_t table_result;
            xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
            xassert(proposal_block != nullptr);
         
            mocktable.do_multi_sign(proposal_block);
            mocktable.on_table_finish(proposal_block);
            resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    }
#endif
}


TEST_F(test_tablemaker, receipt_id_check_1) {
    xblockmaker_resources_ptr_t resources = std::make_shared<test_xblockmaker_resources_t>();

    mock::xdatamock_table mocktable(1, 2);
    std::string table_addr = mocktable.get_account();
    std::vector<std::string> unit_addrs = mocktable.get_unit_accounts();
    std::string from_addr = unit_addrs[0];
    std::string to_addr = mock::xdatamock_address::make_unit_address(base::enum_chain_zone_consensus_index, 9);
    mock::xdatamock_table mocktable2(9, 2);
    std::string table_addr2 = mocktable2.get_account();
    xassert(table_addr != table_addr2);

    resources->get_txpool()->subscribe_tables(0, 1, 1,common::xnode_type_t::consensus_auditor);

    mocktable.store_genesis_units(resources->get_blockstore());
    mocktable2.store_genesis_units(resources->get_blockstore());
    
    xtable_maker_ptr_t tablemaker = make_object_ptr<xtable_maker_t>(table_addr, resources);
    xtable_maker_ptr_t tablemaker2 = make_object_ptr<xtable_maker_t>(table_addr2, resources);

    xblock_ptr_t first_table_block = nullptr;
    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        std::vector<xcons_transaction_ptr_t> send_txs = mocktable.create_send_txs(from_addr, to_addr, 2);
        table_para.set_origin_txs(send_txs);
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 1);

        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());
        first_table_block = proposal_block;
    }
    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 2);

        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    }
    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 3);

        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());
        resources->get_txpool()->on_block_confirmed(first_table_block.get());
    }
    {
        xtablemaker_para_t table_para(mocktable2.get_table_state(), mocktable2.get_commit_table_state());
        auto tableblocks = mocktable.get_history_tables();
        std::vector<xcons_transaction_ptr_t> recv_txs = mocktable.create_receipts(tableblocks[1]);
        xassert(recv_txs.size() == 2);
        std::cout << "recv_txs[0] " << recv_txs[0]->dump() << " " << recv_txs[0]->get_target_addr() << std::endl;
        std::cout << "recv_txs[1] " << recv_txs[1]->dump() << " " << recv_txs[0]->get_target_addr() << std::endl;
        // std::cout << "recv_txs[0]->get_last_action_receipt_id() " << recv_txs[0]->get_last_action_receipt_id() << std::endl;
        // std::cout << "recv_txs[0]->get_last_action_sender_confirmed_receipt_id() " << recv_txs[0]->get_last_action_sender_confirmed_receipt_id() << std::endl;
        // std::cout << "recv_txs[1]->get_last_action_receipt_id() " << recv_txs[1]->get_last_action_receipt_id() << std::endl;
        // std::cout << "recv_txs[1]->get_last_action_sender_confirmed_receipt_id() " << recv_txs[1]->get_last_action_sender_confirmed_receipt_id() << std::endl;    
        xassert(recv_txs[0]->get_last_action_receipt_id() == 1);
        xassert(recv_txs[1]->get_last_action_receipt_id() == 2);

        table_para.set_origin_txs(recv_txs);
        xblock_consensus_para_t proposal_para = mocktable2.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker2->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 1);

        mocktable2.do_multi_sign(proposal_block);
        mocktable2.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable2, proposal_block.get());
    }
    {
        xtablemaker_para_t table_para(mocktable2.get_table_state(), mocktable2.get_commit_table_state());
        xblock_consensus_para_t proposal_para = mocktable2.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker2->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 2);

        mocktable2.do_multi_sign(proposal_block);
        mocktable2.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable2, proposal_block.get());
    }
    {
        xtablemaker_para_t table_para(mocktable2.get_table_state(), mocktable2.get_commit_table_state());
        xblock_consensus_para_t proposal_para = mocktable2.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker2->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 3);

        mocktable2.do_multi_sign(proposal_block);
        mocktable2.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable2, proposal_block.get());
    }
    // {
    //     xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
    //     auto tableblocks = mocktable.get_history_tables();
    //     std::vector<xcons_transaction_ptr_t> confirm_txs = mocktable.create_receipts(tableblocks[4]);
    //     xassert(confirm_txs.size() == 2);
    //     xassert(confirm_txs[0]->get_last_action_receipt_id() == 1);
    //     xassert(confirm_txs[1]->get_last_action_receipt_id() == 2);
    //     std::cout << "confirm_txs[0]->get_last_action_receipt_id() " << confirm_txs[0]->get_last_action_receipt_id() << std::endl;
    //     std::cout << "confirm_txs[0]->get_last_action_sender_confirmed_receipt_id() " << confirm_txs[0]->get_last_action_sender_confirmed_receipt_id() << std::endl;
    //     std::cout << "confirm_txs[1]->get_last_action_receipt_id() " << confirm_txs[1]->get_last_action_receipt_id() << std::endl;
    //     std::cout << "confirm_txs[1]->get_last_action_sender_confirmed_receipt_id() " << confirm_txs[1]->get_last_action_sender_confirmed_receipt_id() << std::endl;    

    //     table_para.set_origin_txs(confirm_txs);
    //     xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

    //     xtablemaker_result_t table_result;
    //     xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
    //     xassert(proposal_block != nullptr);
    //     xassert(proposal_block->get_height() == 7);

    //     mocktable.do_multi_sign(proposal_block);
    //     mocktable.on_table_finish(proposal_block);
    //     resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    // }
    // {
    //     xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
    //     xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

    //     xtablemaker_result_t table_result;
    //     xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
    //     xassert(proposal_block != nullptr);
    //     xassert(proposal_block->get_height() == 8);

    //     mocktable.do_multi_sign(proposal_block);
    //     mocktable.on_table_finish(proposal_block);
    //     resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    // }
    // {
    //     xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
    //     xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

    //     xtablemaker_result_t table_result;
    //     xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
    //     xassert(proposal_block != nullptr);
    //     xassert(proposal_block->get_height() == 9);

    //     mocktable.do_multi_sign(proposal_block);
    //     mocktable.on_table_finish(proposal_block);
    //     resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    // }
#if 0
    {
        xassert(mocktable.get_table_state()->get_block_height() == 6);
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        std::vector<xcons_transaction_ptr_t> send_txs = mocktable.create_send_txs(from_addr, to_addr, 2);
        table_para.set_origin_txs(send_txs);

        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 7);

        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());
        first_table_block = proposal_block;
    }
    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 8);

        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    }
    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 9);

        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());
        resources->get_txpool()->on_block_confirmed(first_table_block.get());
    }

    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        auto tableblocks = mocktable.get_history_tables();
        std::vector<xcons_transaction_ptr_t> recv_txs = mocktable.create_receipts(tableblocks[7]);
        xassert(recv_txs.size() == 2);
        std::cout << "recv_txs[0]->get_last_action_receipt_id() " << recv_txs[0]->get_last_action_receipt_id() << std::endl;
        std::cout << "recv_txs[0]->get_last_action_sender_confirmed_receipt_id() " << recv_txs[0]->get_last_action_sender_confirmed_receipt_id() << std::endl;
        std::cout << "recv_txs[1]->get_last_action_receipt_id() " << recv_txs[1]->get_last_action_receipt_id() << std::endl;
        std::cout << "recv_txs[1]->get_last_action_sender_confirmed_receipt_id() " << recv_txs[1]->get_last_action_sender_confirmed_receipt_id() << std::endl;
        xassert(recv_txs[0]->get_last_action_receipt_id() == 3);
        xassert(recv_txs[1]->get_last_action_receipt_id() == 4);
        // xassert(recv_txs[0]->get_last_action_sender_confirmed_receipt_id() == 2);
        // xassert(recv_txs[1]->get_last_action_sender_confirmed_receipt_id() == 2);

        table_para.set_origin_txs(recv_txs);
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 10);

        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    }
    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 11);

        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    }
    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 12);

        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    }
#endif
    // {
    //     xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
    //     auto tableblocks = mocktable.get_history_tables();
    //     std::vector<xcons_transaction_ptr_t> confirm_txs = mocktable.create_receipts(tableblocks[13]);
    //     xassert(confirm_txs.size() == 2);
    //     std::cout << "confirm_txs[0]->get_last_action_receipt_id() " << confirm_txs[0]->get_last_action_receipt_id() << std::endl;
    //     std::cout << "confirm_txs[0]->get_last_action_sender_confirmed_receipt_id() " << confirm_txs[0]->get_last_action_sender_confirmed_receipt_id() << std::endl;
    //     std::cout << "confirm_txs[1]->get_last_action_receipt_id() " << confirm_txs[1]->get_last_action_receipt_id() << std::endl;
    //     std::cout << "confirm_txs[1]->get_last_action_sender_confirmed_receipt_id() " << confirm_txs[1]->get_last_action_sender_confirmed_receipt_id() << std::endl;    
    //     xassert(confirm_txs[0]->get_last_action_receipt_id() == 3);
    //     xassert(confirm_txs[1]->get_last_action_receipt_id() == 4);

    //     table_para.set_origin_txs(confirm_txs);
    //     xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

    //     xtablemaker_result_t table_result;
    //     xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
    //     xassert(proposal_block != nullptr);
    //     xassert(proposal_block->get_height() == 16);

    //     mocktable.do_multi_sign(proposal_block);
    //     mocktable.on_table_finish(proposal_block);
    //     resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    // }

    // {
    //     xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
    //     auto tableblocks = mocktable.get_history_tables();
    //     std::vector<xcons_transaction_ptr_t> confirm_txs = mocktable.create_receipts(tableblocks[13]);
    //     xassert(confirm_txs.size() == 2);
    //     std::cout << "confirm_txs[0]->get_last_action_receipt_id() " << confirm_txs[0]->get_last_action_receipt_id() << std::endl;
    //     std::cout << "confirm_txs[0]->get_last_action_sender_confirmed_receipt_id() " << confirm_txs[0]->get_last_action_sender_confirmed_receipt_id() << std::endl;
    //     std::cout << "confirm_txs[1]->get_last_action_receipt_id() " << confirm_txs[1]->get_last_action_receipt_id() << std::endl;
    //     std::cout << "confirm_txs[1]->get_last_action_sender_confirmed_receipt_id() " << confirm_txs[1]->get_last_action_sender_confirmed_receipt_id() << std::endl;    
    //     xassert(confirm_txs[0]->get_last_action_receipt_id() == 3);
    //     xassert(confirm_txs[1]->get_last_action_receipt_id() == 4);

    //     table_para.set_origin_txs(confirm_txs);
    //     xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

    //     xtablemaker_result_t table_result;
    //     xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
    //     xassert(proposal_block != nullptr);
    //     xassert(proposal_block->get_height() == 17);

    //     mocktable.do_multi_sign(proposal_block);
    //     mocktable.on_table_finish(proposal_block);
    //     resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    // }
    // {
    //     xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
    //     xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

    //     xtablemaker_result_t table_result;
    //     xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
    //     xassert(proposal_block != nullptr);
    //     xassert(proposal_block->get_height() == 18);

    //     mocktable.do_multi_sign(proposal_block);
    //     mocktable.on_table_finish(proposal_block);
    //     resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    // }
    // {
    //     xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
    //     xblock_consensus_para_t proposal_para = mocktable.init_consensus_para();

    //     xtablemaker_result_t table_result;
    //     xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
    //     xassert(proposal_block != nullptr);
    //     xassert(proposal_block->get_height() == 19);

    //     mocktable.do_multi_sign(proposal_block);
    //     mocktable.on_table_finish(proposal_block);
    //     resources->get_blockstore()->store_block(mocktable, proposal_block.get());
    // }
#if 0
    {
    // full-table 16 height
    auto tableblocks = mocktable.get_history_tables();
    auto tablestate = resources->get_xblkstatestore()->get_block_state(tableblocks[10].get());
{
    base::xvbstate_t* tablestate2 = static_cast<base::xvbstate_t*>(tablestate->clone());
    xassert(tablestate2 != nullptr);

    xvproperty_prove_ptr_t propreceipt2 = xblocktool_t::create_receiptid_property_prove(tableblocks[10].get(), tableblocks[12].get(), tablestate2);
    xassert(propreceipt2 != nullptr);
    xassert(propreceipt2->is_valid());
}

    xvproperty_prove_ptr_t propreceipt = xblocktool_t::create_receiptid_property_prove(tableblocks[10].get(), tableblocks[12].get(), tablestate.get());
    xassert(propreceipt != nullptr);
    xassert(propreceipt->is_valid());

    std::string propreceipt_bin;
    propreceipt->serialize_to_string(propreceipt_bin);
    std::cout << "propreceipt_bin size=" << propreceipt_bin.size() << std::endl;

    base::xstream_t _stream(base::xcontext_t::instance(), (uint8_t *)propreceipt_bin.data(), (uint32_t)propreceipt_bin.size());
    xdataunit_t* _dataunit = xdataunit_t::read_from(_stream);
    xassert(_dataunit != nullptr);
    xvproperty_prove_t* _propreceipt = dynamic_cast<xvproperty_prove_t*>(_dataunit);
    xassert(_propreceipt != nullptr);
    xvproperty_prove_ptr_t propreceipt2;
    propreceipt2.attach(_propreceipt);
    xassert(propreceipt2->is_valid());

    base::xreceiptid_state_ptr_t receiptid_state = xblocktool_t::get_receiptid_from_property_prove(propreceipt2);
    xassert(receiptid_state->get_self_tableid() == mocktable.get_short_table_id());
    xassert(receiptid_state->get_block_height() == 10);
    auto all_pairs = receiptid_state->get_all_receiptid_pairs();
    std::cout << "all_pairs=" << all_pairs->dump() << std::endl;
    }
#endif
}
#if 0
TEST_F(test_tablemaker, version_1) {
    xblockmaker_resources_ptr_t resources = std::make_shared<test_xblockmaker_resources_t>();

    mock::xdatamock_table mocktable(1, 2);
    std::string table_addr = mocktable.get_account();
    std::vector<std::string> unit_addrs = mocktable.get_unit_accounts();
    std::string from_addr = unit_addrs[0];
    std::string to_addr = unit_addrs[1];

    mocktable.store_genesis_units(resources->get_blockstore());

    const uint32_t tx_cnt = 2;
    std::vector<xcons_transaction_ptr_t> send_txs = mocktable.create_send_txs(from_addr, to_addr, tx_cnt);
    xtable_maker_ptr_t tablemaker = make_object_ptr<xtable_maker_t>(table_addr, resources);

    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        table_para.set_origin_txs(send_txs);
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para(100);

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        auto txs = proposal_block->get_txs();
        EXPECT_EQ(txs.size(), tx_cnt);
        EXPECT_EQ(proposal_block->get_block_version(), xvblock_fork_t::get_block_fork_old_version());

        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 1);
        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());

        {
            xJson::Value jv1;
            proposal_block->parse_to_json(jv1, RPC_VERSION_V1);
            auto j_txs = jv1["tableblock"]["units"][from_addr]["lightunit_input"];
            auto tx_hashes = j_txs.getMemberNames();
            for(auto hash : tx_hashes) {
                auto sender = j_txs[hash]["sender"].asString();
                EXPECT_EQ(sender, from_addr);
                auto receiver = j_txs[hash]["receiver"].asString();
                EXPECT_EQ(receiver, to_addr);
                auto tx_consensus_phase = j_txs[hash]["tx_consensus_phase"].asString();
                EXPECT_EQ(tx_consensus_phase, "send");
            }
            auto unit_height = jv1["tableblock"]["units"][from_addr]["unit_height"].asUInt64();
            EXPECT_EQ(unit_height, 1);
        } 
        
        {
            xJson::Value jv2;
            proposal_block->parse_to_json(jv2, RPC_VERSION_V2);
            auto j_txs = jv2["tableblock"]["txs"];
            for(auto tx : j_txs) {
                auto tx_consensus_phase = tx["tx_consensus_phase"].asString();
                EXPECT_EQ(tx_consensus_phase, "send");
            }
            auto units = jv2["tableblock"]["units"];
            for (auto & unit : units) {
                auto unit_height = unit["unit_height"].asUInt64();
                EXPECT_EQ(unit_height, 1);
                auto account = unit["account"].asString();
                EXPECT_EQ(account, from_addr);
            }
        }

        std::vector<xobject_ptr_t<base::xvblock_t>> units;
        proposal_block->extract_sub_blocks(units);        
        EXPECT_EQ(units.size(), 1);
        for (auto & v : units) {
            xobject_ptr_t<data::xblock_t> unit = dynamic_xobject_ptr_cast<data::xblock_t>(v);
            auto txs = unit->get_txs();
            EXPECT_EQ(txs.size(), 0);  // unit has none txs

            {
                xJson::Value jv1;
                unit->parse_to_json(jv1, RPC_VERSION_V1);
                auto j_txs = jv1["lightunit"]["lightunit_input"]["txs"];
                for(auto tx : j_txs) {
                    auto hashes = tx.getMemberNames();
                    for (auto & hash : hashes) {
                        auto tx_consensus_phase = tx[hash]["tx_consensus_phase"].asString();
                        EXPECT_EQ(tx_consensus_phase, "send");
                        auto tx_exec_status = tx[hash]["tx_exec_status"].asString();
                        // EXPECT_EQ(tx_exec_status, "success");
                        EXPECT_EQ(tx_exec_status, "");  // XTODO can't make version before enum_xvblock_fork_version_unit_opt
                    }
                }
            }

            {
                xJson::Value jv2;
                unit->parse_to_json(jv2, RPC_VERSION_V2);
                auto txs = jv2["lightunit"]["lightunit_input"];
                for (auto & tx : txs) {
                    auto tx_consensus_phase = tx["tx_consensus_phase"].asString();
                    EXPECT_EQ(tx_consensus_phase, "send");
                }
            }
         }

        auto headers = proposal_block->get_sub_block_headers();
        EXPECT_EQ(headers.size(), 1);
        for (auto & header : headers) {
            EXPECT_EQ(header->get_extra_data().empty(), false);  // enum_xvblock_fork_version_unit_opt unit has extra
            EXPECT_EQ(header->get_block_version(), xvblock_fork_t::get_block_fork_old_version());
        }
    }
}
#endif
TEST_F(test_tablemaker, table_inner_tx) {
    xblockmaker_resources_ptr_t resources = std::make_shared<test_xblockmaker_resources_t>();

    mock::xdatamock_table mocktable(1, 2);
    std::string table_addr = mocktable.get_account();
    std::vector<std::string> unit_addrs = mocktable.get_unit_accounts();
    std::string from_addr = unit_addrs[0];
    std::string to_addr = unit_addrs[1];

    mocktable.store_genesis_units(resources->get_blockstore());

    const uint32_t tx_cnt = 1;
    std::vector<xcons_transaction_ptr_t> send_txs = mocktable.create_send_txs(from_addr, to_addr, tx_cnt);
    EXPECT_EQ(send_txs.size(), tx_cnt);
    xtable_maker_ptr_t tablemaker = make_object_ptr<xtable_maker_t>(table_addr, resources);

    {
        xtablemaker_para_t table_para(mocktable.get_table_state(), mocktable.get_commit_table_state());
        table_para.set_origin_txs(send_txs);
        xblock_consensus_para_t proposal_para = mocktable.init_consensus_para(10000000);

        xtablemaker_result_t table_result;
        xblock_ptr_t proposal_block = tablemaker->make_proposal(table_para, proposal_para, table_result);
        // auto txs = proposal_block->get_txs();
        // EXPECT_EQ(txs.size(), tx_cnt);
        EXPECT_EQ(proposal_block->get_block_version(), xvblock_fork_t::get_block_fork_new_version());

        xassert(proposal_block != nullptr);
        xassert(proposal_block->get_height() == 1);
        mocktable.do_multi_sign(proposal_block);
        mocktable.on_table_finish(proposal_block);
        resources->get_blockstore()->store_block(mocktable, proposal_block.get());

        {
            xJson::Value jv1;
            proposal_block->parse_to_json(jv1, RPC_VERSION_V1);
            auto j_txs = jv1["tableblock"]["units"][from_addr]["lightunit_input"];
            auto tx_hashes = j_txs.getMemberNames();
            EXPECT_EQ(tx_hashes.empty(), true);
            // TODO(jimmy) rpc compatibility
            // auto unit_height = jv1["tableblock"]["units"][from_addr]["unit_height"].asUInt64();
            // EXPECT_EQ(unit_height, 1);
        }
        
        {
            xJson::Value jv2;
            proposal_block->parse_to_json(jv2, RPC_VERSION_V2);
            auto j_txs = jv2["tableblock"]["txs"];
            EXPECT_EQ(j_txs.size(), 2);
            EXPECT_EQ(j_txs[0]["tx_consensus_phase"].asString(), "send");
            EXPECT_EQ(j_txs[1]["tx_consensus_phase"].asString(), "recv");

            auto units = jv2["tableblock"]["units"];
            // EXPECT_EQ(units.size(), 2);// TODO(jimmy) rpc compatibility
            for (auto & unit : units) {
                auto unit_height = unit["unit_height"].asUInt64();
                EXPECT_EQ(unit_height, 1);
                auto account = unit["account"].asString();
                ASSERT_TRUE(account == from_addr || account == to_addr);
            }
        }

        std::vector<xobject_ptr_t<base::xvblock_t>> units;
        proposal_block->extract_sub_blocks(units);
        EXPECT_EQ(units.size(), 2);
        for (auto & v : units) {
            xobject_ptr_t<data::xblock_t> unit = dynamic_xobject_ptr_cast<data::xblock_t>(v);
            // auto txs = unit->get_txs();
            // EXPECT_EQ(txs.size(), 0);
            
            // {
            //     xJson::Value jv1;
            //     unit->parse_to_json(jv1, RPC_VERSION_V1);
            //     auto j_txs = jv1["lightunit"]["lightunit_input"]["txs"];
            //     for(auto tx : j_txs) {
            //         auto hashes = tx.getMemberNames();
            //         for (auto & hash : hashes) {
            //             auto tx_consensus_phase = tx[hash]["tx_consensus_phase"].asString();
            //             EXPECT_EQ(tx_consensus_phase, "send");
            //         }
            //     }
            // }

            {
                xJson::Value jv2;
                unit->parse_to_json(jv2, RPC_VERSION_V2);
                auto txs = jv2["lightunit"]["lightunit_input"];
                for (auto & tx : txs) {
                    auto tx_consensus_phase = tx["tx_consensus_phase"].asString();
                    EXPECT_EQ(tx_consensus_phase, "send");
                }
            }
        }

        std::vector<xobject_ptr_t<xvblock_t>> sub_blocks;
        proposal_block->extract_sub_blocks(sub_blocks);
        // EXPECT_EQ(headers.size(), 2);
        for (auto & subblock : sub_blocks) {
            EXPECT_EQ(subblock->get_header()->get_extra_data().empty(), true);  // not include tx hashs
            EXPECT_EQ(subblock->get_block_version(), xvblock_fork_t::get_block_fork_new_version());
        }
    }
}

TEST_F(test_tablemaker, fullunit) {
    uint64_t count = 25;
    mock::xdatamock_table mocktable;
    mocktable.genrate_table_chain(count, nullptr);

    auto & tables = mocktable.get_history_tables();
    auto & fullunit_table = tables[21];
    std::vector<xobject_ptr_t<base::xvblock_t>> sub_blocks;
    fullunit_table->extract_sub_blocks(sub_blocks);
    for(auto & unit : sub_blocks) {
        EXPECT_EQ(unit->get_block_class(), enum_xvblock_class_full);
        auto fullunit = dynamic_cast<xfullunit_block_t*>(unit.get());
        if (fullunit != nullptr) {
            xJson::Value jv;
            fullunit->parse_to_json(jv, RPC_VERSION_V2);
            auto txs = jv["fullunit"]["txs"];
            for (auto tx : txs) {
                auto tx_hash = tx["tx_hash"].asString();
                std::cout << tx_hash << std::endl;
                EXPECT_EQ(tx_hash.empty(), false);
            }
        }
    }
}

bool convert_func_all_true(const base::xvaccount_t & account, const base::xaccount_index_t & old_account_index, base::xaccount_index_t & new_account_index) {
    uint64_t i = old_account_index.get_latest_unit_height() - 10;
    std::string unit_hash = std::to_string(i + 100);
    std::string state_hash = std::to_string(i + 200);
    uint64_t nonce = i%5;

    new_account_index = base::xaccount_index_t(old_account_index.get_latest_unit_height(), unit_hash, state_hash, nonce);
    return true;
}

bool convert_func_random_true(const base::xvaccount_t & account, const base::xaccount_index_t & old_account_index, base::xaccount_index_t & new_account_index) {
    bool ret = (rand()%5 == 0);
    if (!ret) {
        return false;
    }
    return convert_func_all_true(account, old_account_index, new_account_index);
}

TEST_F(test_tablemaker, account_index_upgrade) {
    uint32_t account_num = 1000;
    uint64_t height = 10000;
    xaccount_index_upgrade_t account_index_upgrade;

    for (uint32_t test_idx = 0; test_idx < 2; test_idx++) {
        account_index_converter convert_func = convert_func_all_true;
        if (test_idx == 1) {
            account_index_converter convert_func = convert_func_random_true;
        }
        account_index_upgrade.init("Ta0000@0", account_num, height);
        for (uint32_t i = 0; i < account_num; i++) {
            std::string account = "test_account" + std::to_string(i);
            
            base::xaccount_index_t old_account_index(i+ 10, "x", "x", 0);
            account_index_upgrade.add_old_index(account, old_account_index);
        }

        EXPECT_EQ(account_index_upgrade.get_fork_height(), height);

        std::map<std::string, base::xaccount_index_t> new_indexes;
        while(true) {
            auto ret = account_index_upgrade.upgrade(convert_func, 50);
            auto ret1 = account_index_upgrade.get_new_indexes(new_indexes, height);
            EXPECT_EQ(ret, ret1);

            if (ret) {
                break;
            }
        }

        for (auto & index_pair : new_indexes) {
            auto & account = index_pair.first;
            auto & new_account_index = index_pair.second;
            uint64_t h = new_account_index.get_latest_unit_height() - 10;
            std::string account_tmp = "test_account" + std::to_string(h);
            EXPECT_EQ(account_tmp, account);
            std::string unit_hash = std::to_string(h + 100);
            std::string state_hash = std::to_string(h + 200);
            uint64_t nonce = h%5;
            EXPECT_EQ(new_account_index.get_latest_unit_hash(), unit_hash);
            EXPECT_EQ(new_account_index.get_latest_state_hash(), state_hash);
            EXPECT_EQ(new_account_index.get_latest_tx_nonce(), nonce);
        }
        account_index_upgrade.clear();
    }
}

TEST_F(test_tablemaker, account_index_upgrade_tool) {
    mock::xvchain_creator creator(true);
    base::xvblockstore_t* blockstore = creator.get_blockstore();

    uint64_t max_block_height = 10;
    uint64_t account_num = 4;
    mock::xdatamock_table mocktable(1, account_num);
    mocktable.genrate_table_chain(max_block_height, blockstore);
    const std::vector<xblock_ptr_t> & tableblocks = mocktable.get_history_tables();
    xassert(tableblocks.size() == max_block_height + 1);

    for (auto & block : tableblocks) {
        ASSERT_TRUE(blockstore->store_block(mocktable, block.get()));
    }

    auto tableblock = tableblocks[max_block_height - 2];
    
    std::vector<xobject_ptr_t<xvblock_t>> sub_blocks;
    tableblock->extract_sub_blocks(sub_blocks);
    ASSERT_EQ(sub_blocks.size(), account_num);

    std::map<std::string, base::xaccount_index_t> new_indexes;

    for (auto & sub_block : sub_blocks) {
        base::xaccount_index_t old_account_index(sub_block->get_height(), sub_block->get_viewid(), 0, enum_xblock_consensus_flag_committed, sub_block->get_block_class(), sub_block->get_block_type(), false, false);

        base::xvaccount_t account(sub_block->get_account());
        base::xaccount_index_t new_account_index;
        auto ret = xaccount_index_upgrade_tool_t::convert_to_new_account_index(account, old_account_index, new_account_index);
        EXPECT_EQ(ret, true);
        EXPECT_EQ(new_account_index.get_latest_unit_height(), sub_block->get_height());
        EXPECT_EQ(new_account_index.get_latest_unit_hash(), sub_block->get_block_hash());
        auto unitbstate = base::xvchain_t::instance().get_xstatestore()->get_blkstate_store()->get_block_state(sub_block.get());
        data::xunitstate_ptr_t unitstate = std::make_shared<data::xunit_bstate_t>(unitbstate.get());
        auto nonce = unitstate->account_send_trans_number();
        std::string unitstate_bin;
        unitbstate->take_snapshot(unitstate_bin);
        unitbstate->take_snapshot(unitstate_bin);
        std::string statehash = sub_block->get_cert()->hash(unitstate_bin);
        EXPECT_EQ(new_account_index.get_latest_state_hash(), statehash);
        EXPECT_EQ(new_account_index.get_latest_tx_nonce(), nonce);
        new_indexes[sub_block->get_account()] = new_account_index;
    }

    auto tableblock_lock = tableblocks[max_block_height - 1];
    xaccount_index_upgrade_tool_t::update_new_indexes_by_block(new_indexes, tableblock_lock);

    std::vector<xobject_ptr_t<xvblock_t>> sub_blocks_lock;
    tableblock_lock->extract_sub_blocks(sub_blocks_lock);
    ASSERT_EQ(sub_blocks_lock.size(), account_num);

    for (auto & sub_block : sub_blocks_lock) {
        base::xvaccount_t account(sub_block->get_account());
        auto iter = new_indexes.find(sub_block->get_account());
        EXPECT_NE(iter, new_indexes.end());
        auto & account_index = iter->second;
        EXPECT_EQ(account_index.get_latest_unit_height(), sub_block->get_height());
        EXPECT_EQ(account_index.get_latest_unit_hash(), sub_block->get_block_hash());
        auto unitbstate = base::xvchain_t::instance().get_xstatestore()->get_blkstate_store()->get_block_state(sub_block.get());
        data::xunitstate_ptr_t unitstate = std::make_shared<data::xunit_bstate_t>(unitbstate.get());
        auto nonce = unitstate->account_send_trans_number();
        std::string unitstate_bin;
        unitbstate->take_snapshot(unitstate_bin);
        unitbstate->take_snapshot(unitstate_bin);
        std::string statehash = sub_block->get_cert()->hash(unitstate_bin);
        EXPECT_EQ(account_index.get_latest_state_hash(), statehash);
        EXPECT_EQ(account_index.get_latest_tx_nonce(), nonce);
    }
}
