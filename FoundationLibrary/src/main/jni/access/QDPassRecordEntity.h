//
// Created by 胡涛 on 2017/5/12.
//

#ifndef QRCODE_QDPASSRECORDENTITY_H
#define QRCODE_QDPASSRECORDENTITY_H


#include <string>

class QDPassRecordEntity {
public:
    std::string app_user_id;
    std::string pass_type;
    std::string create_time;
    std::string record_index;
    std::string room_id;
    std::string server_id;
    std::string dev_mac;
    std::string pass_result;
};


#endif //QRCODE_QDPASSRECORDENTITY_H
