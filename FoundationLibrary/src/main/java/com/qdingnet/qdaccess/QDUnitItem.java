package com.qdingnet.qdaccess;

public class QDUnitItem {
	public long unit_id;
	public int  multi_time_counter;
	public int  single_time_counter;
	
	public QDUnitItem(long unit_id, int multi_time_counter, int single_time_counter) {
		super();
		this.unit_id = unit_id;
		this.multi_time_counter = multi_time_counter;
		this.single_time_counter = single_time_counter;
	}

	@Override
	public String toString() {
		return "QDUnitItem [unit_id=" + unit_id + ", multi_time_counter=" + multi_time_counter
				+ ", single_time_counter=" + single_time_counter + "]";
	}
	
}
