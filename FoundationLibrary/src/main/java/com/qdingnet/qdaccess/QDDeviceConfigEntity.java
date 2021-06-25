package com.qdingnet.qdaccess;

import java.util.ArrayList;

public class QDDeviceConfigEntity {
	public int open_keep_time;
	public long device_time;
	public String software_version;
	public String hardware_version;
	//V1
	public String related_doors;
	public int max_related_doors_size;
	public int max_single_uinit_counter_size;
	//V2
	public ArrayList<QDUnitItem> related_units;

	public QDDeviceConfigEntity(int open_keep_time, int max_related_doors_size, int max_single_uinit_counter_size, long device_time, String software_version, String hardware_version, String related_doors) {
		this.open_keep_time = open_keep_time;
		this.max_related_doors_size = max_related_doors_size;
		this.max_single_uinit_counter_size = max_single_uinit_counter_size;
		this.device_time = device_time;
		this.software_version = software_version;
		this.hardware_version = hardware_version;
		this.related_doors = related_doors;
	}

	public QDDeviceConfigEntity(int open_keep_time, long device_time, String software_version, String hardware_version, ArrayList<QDUnitItem> related_units) {
		this.open_keep_time = open_keep_time;
		this.device_time = device_time;
		this.software_version = software_version;
		this.hardware_version = hardware_version;
		this.related_units = related_units;
	}
}
