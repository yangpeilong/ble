package com.qdingnet.provider.config.bean;

/**
 * 设备配置单元信息(一二代设备)
 * @author pipan (pipan@qding.me)
 * @date   2019/5/28
 */
public class GateBuildUnitInfo {
    //V1
    public String province;
    public String city;
    public String community;
    public String build;
    public String unit;
    //V2
    public long unit_id;
    public int password_num;

    public GateBuildUnitInfo(String province, String city, String community, String build, String unit, long unit_id, int password_num) {
        this.province = province;
        this.city = city;
        this.community = community;
        this.build = build;
        this.unit = unit;
        this.unit_id = unit_id;
        this.password_num = password_num;
    }

    public GateBuildUnitInfo(String province, String city, String community, String build, String unit) {
        this.province = province;
        this.city = city;
        this.community = community;
        this.build = build;
        this.unit = unit;
    }

    public GateBuildUnitInfo(long unit_id, int password_num) {
        this.unit_id = unit_id;
        this.password_num = password_num;
    }

    @Override
    public String toString() {
        return "GateBuildUnitInfo{" +
                "province='" + province + '\'' +
                ", city='" + city + '\'' +
                ", community='" + community + '\'' +
                ", build='" + build + '\'' +
                ", unit='" + unit + '\'' +
                ", unit_id=" + unit_id +
                ", password_num=" + password_num +
                '}';
    }
}
