package com.qdingnet.qdaccess;

public class QDRFCardItem {
	public long card_no;
	public long timestamp;
	public byte operate_type;
	
	public QDRFCardItem(){}

	public QDRFCardItem(long card_no, long timestamp, byte operate_type) {
		this.card_no = card_no;
		this.timestamp = timestamp;
		this.operate_type = operate_type;
	}

	@Override
	public String toString() {
		return "QDRFCardItem [card_no=" + card_no + ", timestamp=" + timestamp + ", operate_type=" + operate_type + "]";
	}
}
