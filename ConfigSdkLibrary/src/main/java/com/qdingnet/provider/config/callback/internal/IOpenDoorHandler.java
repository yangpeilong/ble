package com.qdingnet.provider.config.callback.internal;

import com.qdingnet.provider.config.bean.QDAccessResult;
import com.qdingnet.qdaccess.QDPassRecordEntity;

import java.util.List;

/**
 * 开门结果回调
 * @author pipan (pipan@qding.me)
 * @date   19-1-28 
 */
public interface IOpenDoorHandler {
    /**
     * 开门结果
     * @param mac -- 门禁设备mac
     * @param passId -- 开门标示id
     * @param result -- 开门结果
     */
    void onOpenDoorResult(String mac, int passId, QDAccessResult result);

    /**
     * 设备端开门记录
     * @param mac
     * @param records
     */
    void onOpenDoorRecords(String mac, List<QDPassRecordEntity> records);

}
