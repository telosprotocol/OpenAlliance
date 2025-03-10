cmake_minimum_required(VERSION 3.8)

aux_source_directory(./xerror error)
aux_source_directory(./prerequest prerequest)
aux_source_directory(./xedge edge)
aux_source_directory(./xcluster cluster)
aux_source_directory(./xgetblock getblock)
aux_source_directory(./xshard shard)
aux_source_directory(./xarc arc)
aux_source_directory(./ xrpc)
aux_source_directory(./xhttp xhttp)
aux_source_directory(./xws xws)
aux_source_directory(./xratelimit xratelimit)
aux_source_directory(./eth_rpc eth_rpc)

add_library(xrpc STATIC ${error} ${prerequest} ${edge} ${cluster} ${getblock} ${shard} ${arc} ${xrpc} ${xhttp} ${xws} ${xratelimit} ${eth_rpc})

add_dependencies(xrpc xevm_contract_runtime xelect_net xtxexecutor xtxstore xconfig xcodec xvnetwork xcommon xrouter xverifier xxbase xxg)

target_include_directories(xrpc PRIVATE ${CMAKE_SOURCE_DIR}/src/xtopcom/xdepends/openssl_include/openssl/)
target_link_libraries(xrpc PRIVATE xevm_contract_runtime xelect_net xtxexecutor xtxstore xconfig msgpackc xcodec xvnetwork xcommon xrouter xverifier xbasic xxbase xxg pthread)

if (BUILD_METRICS)
    #add_dependencies(xrpc xmetrics)
    target_link_libraries(xrpc PRIVATE xmetrics)
endif()



