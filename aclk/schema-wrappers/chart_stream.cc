// SPDX-License-Identifier: GPL-3.0-or-later

#include "../aclk-schemas/proto/chart/v1/stream.pb.h"
#include "chart_stream.h"

#include <sys/time.h>
#include <stdlib.h>

stream_charts_and_dims_t parse_stream_charts_and_dims(const char *data, size_t len)
{
    chart::v1::StreamChartsAndDimensions msg;
    stream_charts_and_dims_t res = { .claim_id = NULL, .node_id = NULL };

    if (!msg.ParseFromArray(data, len))
        return res;

    res.node_id = strdup(msg.node_id().c_str());
    res.claim_id = strdup(msg.claim_id().c_str());
    res.seq_id = msg.sequence_id();
    res.batch_id = msg.batch_id();
    res.seq_id_created_at.tv_usec = msg.seq_id_created_at().nanos() / 1000;
    res.seq_id_created_at.tv_sec = msg.seq_id_created_at().seconds();

    return res;
}

chart_and_dim_ack_t parse_chart_and_dimensions_ack(const char *data, size_t len)
{
    chart::v1::ChartsAndDimensionsAck msg;
    chart_and_dim_ack_t res = { .claim_id = NULL, .node_id = NULL };

    if (!msg.ParseFromArray(data, len))
        return res;

    res.node_id = strdup(msg.node_id().c_str());
    res.claim_id = strdup(msg.claim_id().c_str());
    res.last_seq_id = msg.last_sequence_id();

    return res;
}

char *generate_reset_chart_messages(size_t *len, const chart_reset_reason_t reason)
{
    chart::v1::ResetChartMessages msg;

    switch (reason) {
        case DB_EMPTY:
            msg.set_reason(chart::v1::ResetReason::DB_EMPTY);
            break;
        case SEQ_ID_NOT_EXISTS:
            msg.set_reason(chart::v1::ResetReason::SEQ_ID_NOT_EXISTS);
            break;
        case TIMESTAMP_MISMATCH:
            msg.set_reason(chart::v1::ResetReason::TIMESTAMP_MISMATCH);
            break;
        default:
            return NULL;
    }

    *len = msg.ByteSizeLong();
    char *bin = (char*)malloc(*len);
    if (bin)
        msg.SerializeToArray(bin, *len);

    return bin;
}
