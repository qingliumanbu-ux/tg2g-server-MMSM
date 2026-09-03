/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      夏梦影
Version:     1.0
Date:        2024-1-16 14:13:28
Description: 集控大屏加料数据添加
**************************************************/

#include "stdafx.h"
#include "epex.h"
BM2_FUNCTION_EXPORT
int f_mmsm009d_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0; 
	CString sqlstr = " ";
	CString sql_insert = " ";
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_insert(conn);
	CString station_id = " ";
	CString v_proc_div = " ";
	CModel tmmsm2a("TMMSM2A");
	CModel bof("DA_BOF_CHARGE");//转炉
	CModel aod("DA_AOD_CHARGE");//AOD
	CModel eaf("DA_EAF_CHARGE");//电炉
	CModel cif("DA_IF_CHARGE");//IF
	CModel lts("DA_LTS_CHARGE");//LTS
	CModel lf("DA_LF_CHARGE");//lf
	CModel rh("DA_RH_CHARGE");//RH
	CModel vod("DA_VOD_CHARGE");//vod
	CModel des("DA_DES_CHARGE");//des
	CString id = " ";
	try
	{
		for (int i = 0; i < bcls_rec->Tables["TMMSM2A"].Rows.get_Count(); i++)
		{

			//ID_2A
			station_id = bcls_rec->Tables["TMMSM2A"].Rows[i]["STATION_ID"].ToString().Trim();
			v_proc_div = bcls_rec->Tables["TMMSM2A"].Rows[i]["PROC_DIV"].ToString().Trim();
			tmmsm2a["L2_PROC_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["L2_PROC_NO"].ToString().Trim();
			tmmsm2a["HEAT_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["HEAT_NO"].ToString().Trim();
			tmmsm2a["PROD_SEQ_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["PROD_SEQ_NO"].ToString().Trim();
			tmmsm2a["PROC_COUNT"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["PROC_COUNT"].ToString();
			Log::Trace("", __FUNCTION__, "L2_PROC_NO=[{0}]", tmmsm2a["L2_PROC_NO"].ToString());
			Log::Trace("", __FUNCTION__, "PROD_SEQ_NO=[{0}]", tmmsm2a["PROD_SEQ_NO"].ToString());
			Log::Trace("", __FUNCTION__, "PROC_COUNT=[{0}]", tmmsm2a["PROC_COUNT"].ToString());
			tmmsm2a.Query("L2_PROC_NO,PROD_SEQ_NO,PROC_COUNT");
			tmmsm2a.TrimOrBlank();
			cmd_sql.SetCommandText(" SELECT LPAD(TO_CHAR(TMMSM2A_ID.NEXTVAL), 9, '0') AS ID FROM DUAl ");
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				id = cmd_sql.GetString(1);
			}
			cmd_sql.Close();
			tmmsm2a.Print();
			Log::Trace("", __FUNCTION__, "station_id=[{0}]", station_id);
			Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
			Log::Trace("", __FUNCTION__, "L2_PROC_NO=[{0}]", tmmsm2a["L2_PROC_NO"].ToString());
			if (station_id == "B" || station_id == "Y"){
				if (v_proc_div == "I"){
					//CString id = tmmsm2a["ID_2A"];
					CString aggregate_name = bcls_rec->Tables["TMMSM2A"].Rows[i]["DEV_CODE"].ToString().Trim();
					CString heat_number = tmmsm2a["L2_PROC_NO"];
					//工位
					bof["AGGREGATE_NAME"] = tmmsm2a["DEV_CODE"].ToString();
					//VAI加料消息号
					bof["VAI_CHARGE_ID"] = tmmsm2a["ID_2A"].ToString();
					//计划号
					CString order_number = tmmsm2a["SM_PLAN_NOL2"].ToString();
					//分包标志
					CString split_indication = tmmsm2a["SPLIT_INDICATION"].ToString();
					//处理次数
					CString treatment_counter = tmmsm2a["SAME_PROC_NUM"].ToString();
					//加料类型  B-高位料仓 W-丝线 X-料篮/料槽 M-手投料'
					CString charge_type = tmmsm2a["CHARGE_TYPE"].ToString();
					//对于X标示层号
					//bof["CNT"] = tmmsm2a[""].ToString();
					//物料编码
					CString matid = tmmsm2a["MAT_CODE"].ToString();
					//物料描述
					CString description = tmmsm2a["MAT_NAME"].ToString();
					//计量单号
					//bof["MEASUREID"] = tmmsm2a[""].ToString();
					//料槽/料篮/高位料仓号, MES不管理的仓位为VAI_XXXX
					CString stkid = tmmsm2a["STK_NO"].ToString();
					//MES原料减库处理状态
					//bof["STATUS"] = tmmsm2a[""].ToString();
					//原材料L2发送数据时间
					CDateTime send_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
					bof["SENDTIME"] = send_time;
					//MES接收数据时间
					CDateTime receivetime = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
					bof["RECEIVETIME"] = receivetime;
					//MES原料减库处理时间
					//bof["PROCESSTIME"] = tmmsm2a[""].ToString();
					//重量，KG'
					CString weight = tmmsm2a["DEVO_WT"].ToString();
					sql_insert = " insert into DA_BOF_CHARGE (ID, AGGREGATE_NAME, HEAT_NUMBER, ORDER_NUMBER, SPLIT_INDICATION, "
						" TREATMENT_COUNTER, CHARGE_TYPE,  MATID, DESCRIPTION, RECEIVETIME, WEIGHT,timestamp,VAI_CHARGE_ID,STKID) "
						" values('" + id + "','" + aggregate_name + "','" + heat_number + "','" + order_number + "','" + split_indication + "', "
						" '" + treatment_counter + "','" + charge_type + "','" + matid + "','" + description + "',to_date('" + tmmsm2a["REC_CREATE_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + weight + "', "
						" to_date('" + tmmsm2a["REC_CREATE_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + bof["VAI_CHARGE_ID"].ToString() + "','" + stkid + "' "
						" ) ";
					cmd_insert.SetCommandText(sql_insert);
					cmd_insert.Parameters.Set("id", id);
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.ExecuteNonQuery();
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.Close();
				}
				else if (v_proc_div == "D"){
					tmmsm2a["L2_PROC_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["L2_PROC_NO"].ToString().Trim();
					tmmsm2a["HEAT_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["HEAT_NO"].ToString().Trim();
					tmmsm2a["PROD_SEQ_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["PROD_SEQ_NO"].ToString().Trim();
					tmmsm2a.Query("L2_PROC_NO,PROD_SEQ_NO");
					bof["AGGREGATE_NAME"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["DEV_CODE"].ToString().Trim();
					bof["HEAT_NUMBER"] = tmmsm2a["HEAT_NO"];
					if (bof.QueryCount("AGGREGATE_NAME,HEAT_NUMBER"))
					{
						bof.Delete("L2_PROC_NO,HEAT_NO");
					}
				}
			}
			else if (station_id == "A"){
				if (v_proc_div == "I" || v_proc_div == "U"){
					Log::Trace("", __FUNCTION__, "id=[{0}]", id);
					Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
					aod["AGGREGATE_NAME"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["DEV_CODE"].ToString().Trim();
					CString HEAT_NUMBER = tmmsm2a["L2_PROC_NO"];

					//关键字--未定
					//工位
					CString aggregate_name = tmmsm2a["DEV_CODE"].ToString();
					//VAI加料消息号
					CString vai_charge_id = tmmsm2a["ID_2A"].ToString();
					//炉次号
					CString heat_number = tmmsm2a["L2_PROC_NO"].ToString();
					//计划号
					CString order_number = tmmsm2a["SM_PLAN_NOL2"].ToString();
					//分包标志
					CString split_indication = tmmsm2a["SPLIT_INDICATION"].ToString();
					//处理次数
					CString treatment_counter = tmmsm2a["SAME_PROC_NUM"].ToString();
					//加料类型 B-高位料仓 W-丝线 X-料篮/料槽 M-手投料
					CString charge_type = tmmsm2a["CHARGE_TYPE"].ToString();
					//对于X标示层号, 对于其他表示顺序或次数
					//aod["CNT"] = tmmsm2a[""].ToString();
					//物料编码, 可以连接STD_MATERIAL取得DESCRIPTION
					CString matid = tmmsm2a["MAT_CODE"].ToString();
					//物料描述
					CString description = tmmsm2a["MAT_NAME"].ToString();
					//计量单号
					//aod["MEASUREID"] = tmmsm2a[""].ToString();
					//料槽/料篮/高位料仓号, MES不管理的仓位为VAI_XXXX
					CString stkid = tmmsm2a["STK_NO"].ToString();
					//MES原料减库处理状态, 0-未处理 1-已处理
					//aod["STATUS"] = tmmsm2a[""].ToString();
					//原材料L2发送数据时间
					CDateTime send_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
					aod["SENDTIME"] = send_time;
					//MES接收数据时间
					CDateTime receive_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
					aod["RECEIVETIME"] = receive_time;
					//MES原料减库处理时间
					//aod["PROCESSTIME"] = tmmsm2a[""].ToString();
					//MES原料减库处理错误消息
					//aod["ERRMSG"] = tmmsm2a[""].ToString();
					//重量，KG
					CString weight = tmmsm2a["DEVO_WT"].ToString();
					sql_insert = " insert into DA_AOD_CHARGE (ID, AGGREGATE_NAME, HEAT_NUMBER, ORDER_NUMBER, SPLIT_INDICATION, "
						" TREATMENT_COUNTER, CHARGE_TYPE, MATID, DESCRIPTION, STKID, SENDTIME, "
						" RECEIVETIME, WEIGHT,TIMESTAMP,VAI_CHARGE_ID) "
						" values('" + id + "','" + aggregate_name + "','" + heat_number + "','" + order_number + "','" + split_indication + "', "
						" '" + treatment_counter + "','" + charge_type + "','" + matid + "','" + description + "','" + stkid + "',to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'), "
						" to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'),'" + weight + "', "
						" to_date('" + tmmsm2a["REC_CREATE_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + vai_charge_id + "' "
						" ) ";
					cmd_insert.SetCommandText(sql_insert);
					cmd_insert.Parameters.Set("id", id);
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.ExecuteNonQuery();
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.Close();
				}
				else if (v_proc_div == "D"){
					tmmsm2a["L2_PROC_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["L2_PROC_NO"].ToString().Trim();
					tmmsm2a["HEAT_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["HEAT_NO"].ToString().Trim();
					tmmsm2a.Query("L2_PROC_NO,HEAT_NO");
					aod["AGGREGATE_NAME"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["DEV_CODE"].ToString().Trim();
					aod["HEAT_NUMBER"] = tmmsm2a["HEAT_NO"];
					if (aod.QueryCount("AGGREGATE_NAME,HEAT_NUMBER"))
					{
						aod.Delete("L2_PROC_NO,HEAT_NO");
					}
				}
			}
			else if (station_id == "E"){
				if (v_proc_div == "I" || v_proc_div == "U"){
					Log::Trace("", __FUNCTION__, "id=[{0}]", id);
					CString heat_number = tmmsm2a["L2_PROC_NO"];
					//工位
					CString aggregate_name = tmmsm2a["DEV_CODE"].ToString();
					//VAI加料消息号
					CString vai_charge_id = tmmsm2a["ID_2A"].ToString();
					//计划号
					CString order_number = tmmsm2a["SM_PLAN_NOL2"].ToString();
					//分包标志
					CString split_indication = tmmsm2a["SPLIT_INDICATION"].ToString();
					//处理次数
					CString treatment_counter = tmmsm2a["SAME_PROC_NUM"].ToString();
					//加料类型 B-高位料仓 W-丝线 X-料篮/料槽 M-手投料
					CString charge_type = tmmsm2a["CHARGE_TYPE"].ToString();
					//对于X标示层号, 对于其他表示顺序或次数
					//eaf["CNT"] = tmmsm2a[""].ToString();
					//物料编码, 可以连接STD_MATERIAL取得DESCRIPTION
					CString matid = tmmsm2a["MAT_CODE"].ToString();
					//物料描述
					CString description = tmmsm2a["MAT_NAME"].ToString();
					//计量单号
					//eaf["MEASUREID"] = tmmsm2a[""].ToString();
					//料槽/料篮/高位料仓号, MES不管理的仓位为VAI_XXXX
					CString stkid = tmmsm2a["STK_NO"].ToString();
					//MES原料减库处理状态, 0-未处理 1-已处理
					//eaf["STATUS"] = tmmsm2a[""].ToString();
					//原材料L2发送数据时间
					CDateTime send_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
					eaf["SENDTIME"] = send_time;
					//MES接收数据时间
					CDateTime receive_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
					eaf["RECEIVETIME"] = receive_time;
					//MES原料减库处理时间
					//eaf["PROCESSTIME"] = tmmsm2a[""].ToString();
					//MES原料减库处理错误消息
					//eaf["ERRMSG"] = tmmsm2a[""].ToString();
					//重量，KG
					CString weight = tmmsm2a["DEVO_WT"].ToString();
					sql_insert = " insert into DA_EAF_CHARGE (ID, AGGREGATE_NAME, HEAT_NUMBER, ORDER_NUMBER, SPLIT_INDICATION, "
						" TREATMENT_COUNTER, CHARGE_TYPE, MATID, DESCRIPTION, STKID, SENDTIME, "
						" RECEIVETIME, WEIGHT,TIMESTAMP,VAI_CHARGE_ID) "
						" values('" + id + "','" + aggregate_name + "','" + heat_number + "','" + order_number + "','" + split_indication + "', "
						" '" + treatment_counter + "','" + charge_type + "','" + matid + "','" + description + "','" + stkid + "',to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'), "
						" to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'),'" + weight + "', "
						" to_date('" + tmmsm2a["REC_CREATE_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + vai_charge_id + "' "
						" ) ";
					cmd_insert.SetCommandText(sql_insert);
					cmd_insert.Parameters.Set("id", id);
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.ExecuteNonQuery();
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.Close();
				}
				else if (v_proc_div == "D"){
					tmmsm2a["L2_PROC_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["L2_PROC_NO"].ToString().Trim();
					tmmsm2a["HEAT_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["HEAT_NO"].ToString().Trim();
					tmmsm2a.Query("L2_PROC_NO,HEAT_NO");
					eaf["AGGREGATE_NAME"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["DEV_CODE"].ToString().Trim();
					eaf["HEAT_NUMBER"] = tmmsm2a["HEAT_NO"];
					if (eaf.QueryCount("AGGREGATE_NAME,HEAT_NUMBER"))
					{
						eaf.Delete("L2_PROC_NO,HEAT_NO");
					}
				}
			}
			else if (station_id == "Z"){
				if (v_proc_div == "I" || v_proc_div == "U"){
					Log::Trace("", __FUNCTION__, "id=[{0}]", id);
					CString aggregate_name = tmmsm2a["DEV_CODE"].ToString();
					//VAI加料消息号
					CString vai_charge_id = tmmsm2a["ID_2A"].ToString();
					//炉次号
					CString heat_number = tmmsm2a["L2_PROC_NO"].ToString();
					//计划号
					CString order_number = tmmsm2a["SM_PLAN_NOL2"].ToString();
					//分包标志
					CString split_indication = tmmsm2a["SPLIT_INDICATION"].ToString();
					Log::Trace("", __FUNCTION__, "split_indication=[{0}]", split_indication);
					//处理次数
					CString treatment_counter = tmmsm2a["SAME_PROC_NUM"].ToString();
					//加料类型 B-高位料仓 W-丝线 X-料篮/料槽 M-手投料
					CString charge_type = tmmsm2a["CHARGE_TYPE"].ToString();
					//对于X标示层号, 对于其他表示顺序或次数
					//cif["CNT"] = tmmsm2a[""].ToString();
					//物料编码, 可以连接STD_MATERIAL取得DESCRIPTION
					CString matid = tmmsm2a["MAT_CODE"].ToString();
					//物料描述
					CString description = tmmsm2a["MAT_NAME"].ToString();
					//料槽/料篮/高位料仓号, MES不管理的仓位为VAI_XXXX
					CString stkid = tmmsm2a["STK_NO"].ToString();
					//重量，KG
					CString weight = tmmsm2a["DEVO_WT"].ToString();
					sql_insert = " insert into DA_IF_CHARGE (ID, AGGREGATE_NAME, HEAT_NUMBER, ORDER_NUMBER, SPLIT_INDICATION, "
						" TREATMENT_COUNTER, CHARGE_TYPE, MATID, DESCRIPTION, STKID, SENDTIME, "
						" RECEIVETIME, WEIGHT,TIMESTAMP,VAI_CHARGE_ID) "
						" values('" + id + "','" + aggregate_name + "','" + heat_number + "','" + order_number + "','" + split_indication + "', "
						" '" + treatment_counter + "','" + charge_type + "','" + matid + "','" + description + "','" + stkid + "',to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'), "
						" to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'),'" + weight + "', "
						" to_date('" + tmmsm2a["REC_CREATE_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + vai_charge_id + "' "
						" ) ";
					cmd_insert.SetCommandText(sql_insert);
					cmd_insert.Parameters.Set("id", id);
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.ExecuteNonQuery();
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.Close();
				}
				else if (v_proc_div == "D"){
					tmmsm2a["L2_PROC_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["L2_PROC_NO"].ToString().Trim();
					tmmsm2a["HEAT_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["HEAT_NO"].ToString().Trim();
					tmmsm2a.Query("L2_PROC_NO,HEAT_NO");
					cif["AGGREGATE_NAME"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["DEV_CODE"].ToString().Trim();
					cif["HEAT_NUMBER"] = tmmsm2a["HEAT_NO"];
					if (cif.QueryCount("AGGREGATE_NAME,HEAT_NUMBER"))
					{
						cif.Delete("L2_PROC_NO,HEAT_NO");
					}
				}
			}
			else if (station_id == "S"){
				if (v_proc_div == "I" || v_proc_div == "U"){
					Log::Trace("", __FUNCTION__, "id=[{0}]", id);
					CString aggregate_name = bcls_rec->Tables["TMMSM2A"].Rows[i]["DEV_CODE"].ToString().Trim();
					//VAI加料消息号
					lts["VAI_CHARGE_ID"] = tmmsm2a["ID_2A"].ToString();
					//炉次号
					CString heat_number = tmmsm2a["L2_PROC_NO"].ToString();
					//计划号
					CString order_number = tmmsm2a["SM_PLAN_NOL2"].ToString();
					//分包标志
					CString split_indication = tmmsm2a["SPLIT_INDICATION"].ToString();
					//处理次数
					CString treatment_counter = tmmsm2a["SAME_PROC_NUM"].ToString();
					//加料类型 B-高位料仓 W-丝线 X-料篮/料槽 M-手投料
					CString charge_type = tmmsm2a["CHARGE_TYPE"].ToString();
					//对于X标示层号, 对于其他表示顺序或次数
					//lts["CNT"] = tmmsm2a[""].ToString();
					//物料编码, 可以连接STD_MATERIAL取得DESCRIPTION
					CString matid = tmmsm2a["MAT_CODE"].ToString();
					//物料描述
					CString description = tmmsm2a["MAT_NAME"].ToString();
					//计量单号
					//lts["MEASUREID"] = tmmsm2a[""].ToString();
					//料槽/料篮/高位料仓号, MES不管理的仓位为VAI_XXXX
					CString stkid = tmmsm2a["STK_NO"].ToString();
					//MES原料减库处理状态, 0-未处理 1-已处理
					//lts["STATUS"] = tmmsm2a[""].ToString();
					//原材料L2发送数据时间
					CDateTime send_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
					lts["SENDTIME"] = send_time;
					//MES接收数据时间
					CDateTime receive_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
					lts["RECEIVETIME"] = receive_time;
					//MES原料减库处理时间
					//lts["PROCESSTIME"] = tmmsm2a[""].ToString();
					//MES原料减库处理错误消息
					//lts["ERRMSG"] = tmmsm2a[""].ToString();
					//重量，KG
					CString weight = tmmsm2a["DEVO_WT"].ToString();
					sql_insert = " insert into DA_LTS_CHARGE (ID, AGGREGATE_NAME, HEAT_NUMBER, ORDER_NUMBER, SPLIT_INDICATION, "
						" TREATMENT_COUNTER, CHARGE_TYPE, MATID, DESCRIPTION, STKID, SENDTIME, "
						" RECEIVETIME, WEIGHT,TIMESTAMP,VAI_CHARGE_ID) "
						" values('" + id + "','" + aggregate_name + "','" + heat_number + "','" + order_number + "','" + split_indication + "', "
						" '" + treatment_counter + "','" + charge_type + "','" + matid + "','" + description + "','" + stkid + "',to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'), "
						" to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'),'" + weight + "', "
						" to_date('" + tmmsm2a["REC_CREATE_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + lts["VAI_CHARGE_ID"].ToString() + "' "
						" ) ";
					cmd_insert.SetCommandText(sql_insert);
					cmd_insert.Parameters.Set("id", id);
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.ExecuteNonQuery();
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.Close();
				}
				else if (v_proc_div == "D"){
					tmmsm2a["L2_PROC_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["L2_PROC_NO"].ToString().Trim();
					tmmsm2a["HEAT_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["HEAT_NO"].ToString().Trim();
					tmmsm2a.Query("L2_PROC_NO,HEAT_NO");
					lts["AGGREGATE_NAME"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["DEV_CODE"].ToString().Trim();
					lts["HEAT_NUMBER"] = tmmsm2a["HEAT_NO"];
					if (lts.QueryCount("AGGREGATE_NAME,HEAT_NUMBER"))
					{
						lts.Delete("L2_PROC_NO,HEAT_NO");
					}
				}
			}//F
			else if (station_id == "F"){
				if (v_proc_div == "I" || v_proc_div == "U"){
					Log::Trace("", __FUNCTION__, "id=[{0}]", id);
					//工位
					CString aggregate_name = tmmsm2a["DEV_CODE"].ToString();
					//VAI加料消息号
					CString vai_charge_id = tmmsm2a["ID_2A"].ToString();
					//炉次号
					CString heat_number = tmmsm2a["L2_PROC_NO"].ToString();
					//计划号
					CString order_number = tmmsm2a["SM_PLAN_NOL2"].ToString();
					//分包标志
					CString split_indication = tmmsm2a["SPLIT_INDICATION"].ToString();
					//处理次数
					CString treatment_counter = tmmsm2a["SAME_PROC_NUM"].ToString();
					//加料类型 B-高位料仓 W-丝线 X-料篮/料槽 M-手投料
					CString charge_type = tmmsm2a["CHARGE_TYPE"].ToString();
					//对于X标示层号, 对于其他表示顺序或次数
					//lf["CNT"] = tmmsm2a[""].ToString();
					//物料编码, 可以连接STD_MATERIAL取得DESCRIPTION
					CString matid = tmmsm2a["MAT_CODE"].ToString();
					//物料描述
					CString description = tmmsm2a["MAT_NAME"].ToString();
					//计量单号
					//lf["MEASUREID"] = tmmsm2a[""].ToString();
					//料槽/料篮/高位料仓号, MES不管理的仓位为VAI_XXXX
					CString stkid = tmmsm2a["STK_NO"].ToString();
					//MES原料减库处理状态, 0-未处理 1-已处理
					//lf["STATUS"] = tmmsm2a[""].ToString();
					//原材料L2发送数据时间
					CDateTime send_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
					lf["SENDTIME"] = send_time;
					//MES接收数据时间
					CDateTime receive_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
					lf["RECEIVETIME"] = receive_time;
					//MES原料减库处理时间
					//lf["PROCESSTIME"] = tmmsm2a[""].ToString();
					//MES原料减库处理错误消息
					//lf["ERRMSG"] = tmmsm2a[""].ToString();
					//重量，KG
					CString weight = tmmsm2a["DEVO_WT"].ToString();
					sql_insert = " insert into DA_LF_CHARGE (ID, AGGREGATE_NAME, HEAT_NUMBER, ORDER_NUMBER, SPLIT_INDICATION, "
						" TREATMENT_COUNTER, CHARGE_TYPE, MATID, DESCRIPTION, STKID, SENDTIME, "
						" RECEIVETIME, WEIGHT,TIMESTAMP,VAI_CHARGE_ID) "
						" values('" + id + "','" + aggregate_name + "','" + heat_number + "','" + order_number + "','" + split_indication + "', "
						" '" + treatment_counter + "','" + charge_type + "','" + matid + "','" + description + "','" + stkid + "',to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'), "
						" to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'),'" + weight + "', "
						" to_date('" + tmmsm2a["REC_CREATE_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + vai_charge_id + "' "
						" ) ";
					cmd_insert.SetCommandText(sql_insert);
					cmd_insert.Parameters.Set("id", id);
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.ExecuteNonQuery();
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.Close();
				}
				else if (v_proc_div == "D"){
					tmmsm2a["L2_PROC_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["L2_PROC_NO"].ToString().Trim();
					tmmsm2a["HEAT_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["HEAT_NO"].ToString().Trim();
					tmmsm2a.Query("L2_PROC_NO,HEAT_NO");
					lf["AGGREGATE_NAME"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["DEV_CODE"].ToString().Trim();
					lf["HEAT_NUMBER"] = tmmsm2a["HEAT_NO"];
					if (lf.QueryCount("AGGREGATE_NAME,HEAT_NUMBER"))
					{
						lf.Delete("L2_PROC_NO,HEAT_NO");
					}
				}
			}
			else if (station_id == "R"){
				if (v_proc_div == "I" || v_proc_div == "U"){
					Log::Trace("", __FUNCTION__, "id=[{0}]", id);
					//工位
					CString aggregate_name = tmmsm2a["DEV_CODE"].ToString();
					//VAI加料消息号
					CString vai_charge_id = tmmsm2a["ID_2A"].ToString();
					//炉次号
					CString heat_number = tmmsm2a["L2_PROC_NO"].ToString();
					//计划号
					CString order_number = tmmsm2a["SM_PLAN_NOL2"].ToString();
					//分包标志
					CString split_indication = tmmsm2a["SPLIT_INDICATION"].ToString();
					//处理次数
					CString treatment_counter = tmmsm2a["SAME_PROC_NUM"].ToString();
					//加料类型 B-高位料仓 W-丝线 X-料篮/料槽 M-手投料
					CString charge_type = tmmsm2a["CHARGE_TYPE"].ToString();
					//对于X标示层号, 对于其他表示顺序或次数
					//rh["CNT"] = tmmsm2a[""].ToString();
					//物料编码, 可以连接STD_MATERIAL取得DESCRIPTION
					CString matid = tmmsm2a["MAT_CODE"].ToString();
					//物料描述
					CString description = tmmsm2a["MAT_NAME"].ToString();
					//计量单号
					//rh["MEASUREID"] = tmmsm2a[""].ToString();
					//料槽/料篮/高位料仓号, MES不管理的仓位为VAI_XXXX
					CString stkid = tmmsm2a["STK_NO"].ToString();
					//MES原料减库处理状态, 0-未处理 1-已处理
					//rh["STATUS"] = tmmsm2a[""].ToString();
					//原材料L2发送数据时间
					CDateTime send_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
					rh["SENDTIME"] = send_time;
					//MES接收数据时间
					CDateTime receive_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
					rh["RECEIVETIME"] = receive_time;
					//MES原料减库处理时间
					//rh["PROCESSTIME"] = tmmsm2a[""].ToString();
					//MES原料减库处理错误消息
					//rh["ERRMSG"] = tmmsm2a[""].ToString();
					//重量，KG
					CString weight = tmmsm2a["DEVO_WT"].ToString();
					sql_insert = " insert into DA_RH_CHARGE (ID, AGGREGATE_NAME, HEAT_NUMBER, ORDER_NUMBER, SPLIT_INDICATION, "
						" TREATMENT_COUNTER, CHARGE_TYPE, MATID, DESCRIPTION, STKID, SENDTIME, "
						" RECEIVETIME, WEIGHT,TIMESTAMP,VAI_CHARGE_ID) "
						" values('" + id + "','" + aggregate_name + "','" + heat_number + "','" + order_number + "','" + split_indication + "', "
						" '" + treatment_counter + "','" + charge_type + "','" + matid + "','" + description + "','" + stkid + "',to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'), "
						" to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'),'" + weight + "', "
						" to_date('" + tmmsm2a["REC_CREATE_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + vai_charge_id + "' "
						" ) ";
					cmd_insert.SetCommandText(sql_insert);
					cmd_insert.Parameters.Set("id", id);
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.ExecuteNonQuery();
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.Close();
				}
				else if (v_proc_div == "D"){
					tmmsm2a["L2_PROC_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["L2_PROC_NO"].ToString().Trim();
					tmmsm2a["HEAT_NO"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["HEAT_NO"].ToString().Trim();
					tmmsm2a.Query("L2_PROC_NO,HEAT_NO");
					rh["AGGREGATE_NAME"] = bcls_rec->Tables["TMMSM2A"].Rows[i]["DEV_CODE"].ToString().Trim();
					rh["HEAT_NUMBER"] = tmmsm2a["HEAT_NO"];
					if (rh.QueryCount("AGGREGATE_NAME,HEAT_NUMBER"))
					{
						rh.Delete("L2_PROC_NO,HEAT_NO");
					}
				}
			}
			else if (station_id == "V"){
				if (v_proc_div == "I" || v_proc_div == "U"){
					Log::Trace("", __FUNCTION__, "id=[{0}]", id);
					//工位
					CString aggregate_name = tmmsm2a["DEV_CODE"].ToString();
					//VAI加料消息号
					CString vai_charge_id = tmmsm2a["ID_2A"].ToString();
					//炉次号
					CString heat_number = tmmsm2a["L2_PROC_NO"].ToString();
					//计划号
					CString order_number = tmmsm2a["SM_PLAN_NOL2"].ToString();
					//分包标志
					CString split_indication = tmmsm2a["SPLIT_INDICATION"].ToString();
					//处理次数
					CString treatment_counter = tmmsm2a["SAME_PROC_NUM"].ToString();
					//加料类型 B-高位料仓 W-丝线 X-料篮/料槽 M-手投料
					CString charge_type = tmmsm2a["CHARGE_TYPE"].ToString();
					//对于X标示层号, 对于其他表示顺序或次数
					//vod["CNT"] = tmmsm2a[""].ToString();
					//物料编码, 可以连接STD_MATERIAL取得DESCRIPTION
					CString matid = tmmsm2a["MAT_CODE"].ToString();
					//物料描述
					CString description = tmmsm2a["MAT_NAME"].ToString();
					//计量单号
					//vod["MEASUREID"] = tmmsm2a[""].ToString();
					//料槽/料篮/高位料仓号, MES不管理的仓位为VAI_XXXX
					CString stkid = tmmsm2a["STK_NO"].ToString();
					//MES原料减库处理状态, 0-未处理 1-已处理
					//vod["STATUS"] = tmmsm2a[""].ToString();
					//原材料L2发送数据时间
					CDateTime send_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
					vod["SENDTIME"] = send_time;
					//MES接收数据时间
					CDateTime receive_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
					vod["RECEIVETIME"] = receive_time;
					//MES原料减库处理时间
					//vod["PROCESSTIME"] = tmmsm2a[""].ToString();
					//MES原料减库处理错误消息
					//vod["ERRMSG"] = tmmsm2a[""].ToString();
					//重量，KG
					CString weight = tmmsm2a["DEVO_WT"].ToString();
					sql_insert = " insert into DA_VOD_CHARGE (ID, AGGREGATE_NAME, HEAT_NUMBER, ORDER_NUMBER, SPLIT_INDICATION, "
						" TREATMENT_COUNTER, CHARGE_TYPE, MATID, DESCRIPTION, STKID, SENDTIME, "
						" RECEIVETIME, WEIGHT,TIMESTAMP,VAI_CHARGE_ID) "
						" values('" + id + "','" + aggregate_name + "','" + heat_number + "','" + order_number + "','" + split_indication + "', "
						" '" + treatment_counter + "','" + charge_type + "','" + matid + "','" + description + "','" + stkid + "',to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'), "
						" to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'),'" + weight + "', "
						" to_date('" + tmmsm2a["REC_CREATE_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + vai_charge_id + "' "
						" ) ";
					cmd_insert.SetCommandText(sql_insert);
					cmd_insert.Parameters.Set("id", id);
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.ExecuteNonQuery();
					Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
					cmd_insert.Close();
				}
			}
			else if (station_id == "D"){
				Log::Trace("", __FUNCTION__, "id=[{0}]", id);
				//工位
				CString aggregate_name = tmmsm2a["DEV_CODE"].ToString();
				//VAI加料消息号
				CString vai_charge_id = tmmsm2a["ID_2A"].ToString();
				//炉次号
				CString heat_number = tmmsm2a["L2_PROC_NO"].ToString();
				//计划号
				CString order_number = tmmsm2a["SM_PLAN_NOL2"].ToString();
				//分包标志
				CString split_indication = tmmsm2a["SPLIT_INDICATION"].ToString();
				//处理次数
				CString treatment_counter = tmmsm2a["SAME_PROC_NUM"].ToString();
				//加料类型 B-高位料仓 W-丝线 X-料篮/料槽 M-手投料
				CString charge_type = tmmsm2a["CHARGE_TYPE"].ToString();
				//对于X标示层号, 对于其他表示顺序或次数
				//vod["CNT"] = tmmsm2a[""].ToString();
				//物料编码, 可以连接STD_MATERIAL取得DESCRIPTION
				CString matid = tmmsm2a["MAT_CODE"].ToString();
				//物料描述
				CString description = tmmsm2a["MAT_NAME"].ToString();
				//计量单号
				//vod["MEASUREID"] = tmmsm2a[""].ToString();
				//料槽/料篮/高位料仓号, MES不管理的仓位为VAI_XXXX
				CString stkid = tmmsm2a["STK_NO"].ToString();
				//MES原料减库处理状态, 0-未处理 1-已处理
				//vod["STATUS"] = tmmsm2a[""].ToString();
				//原材料L2发送数据时间
				CDateTime send_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
				vod["SENDTIME"] = send_time;
				//MES接收数据时间
				CDateTime receive_time = CDateTime::Parse(tmmsm2a["REC_CREATE_TIME"].ToString());
				vod["RECEIVETIME"] = receive_time;
				//MES原料减库处理时间
				//vod["PROCESSTIME"] = tmmsm2a[""].ToString();
				//MES原料减库处理错误消息
				//vod["ERRMSG"] = tmmsm2a[""].ToString();
				//重量，KG
				CString weight = tmmsm2a["DEVO_WT"].ToString();
				sql_insert = " insert into DA_DES_CHARGE (ID, AGGREGATE_NAME, HEAT_NUMBER, ORDER_NUMBER, SPLIT_INDICATION, "
					" TREATMENT_COUNTER, CHARGE_TYPE, MATID, DESCRIPTION, STKID, SENDTIME, "
					" RECEIVETIME, WEIGHT,TIMESTAMP,VAI_CHARGE_ID) "
					" values('" + id + "','" + aggregate_name + "','" + heat_number + "','" + order_number + "','" + split_indication + "', "
					" '" + treatment_counter + "','" + charge_type + "','" + matid + "','" + description + "','" + stkid + "',to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'), "
					" to_date(" + tmmsm2a["REC_CREATE_TIME"].ToString() + ", 'yyyy-MM-dd hh24:MI:SS'),'" + weight + "', "
					" to_date('" + tmmsm2a["REC_CREATE_TIME"].ToString() + "', 'yyyy-MM-dd hh24:MI:SS'),'" + vai_charge_id + "' "
					" ) ";
				cmd_insert.SetCommandText(sql_insert);
				cmd_insert.Parameters.Set("id", id);
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.ExecuteNonQuery();
				Log::Trace("", "", "sql_insert = [{0}]", sql_insert);
				cmd_insert.Close();
			}
		}
		

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}