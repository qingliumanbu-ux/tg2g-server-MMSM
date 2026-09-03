/*************************************************
Copyright:		Baosight Software LTD.co Copyright (c) 2013
Author:
Version:		1.0
Date:
Description:
**************************************************/

//框架头文件
#include "stdafx.h"
//#include "tmmsm55a.h"
/*<remark>=========================================================
/**
*                             _ooOoo_
*                            o8888888o
*                            88" . "88
*                            (| -_- |)
*                            O\  =  /O
*                         ____/`---'\____
*                       .'  \\|     |//  `.
*                      /  \\|||  :  |||//  \
*                     /  _||||| -:- |||||-  \
*                     |   | \\\  -  /// |   |
*                     | \_|  ''\---/''  |   |
*                     \  .-\__  `-`  ___/-. /
*                   ___`. .'  /--.--\  `. . __
*                ."" '<  `.___\_<|>_/___.'  >'"".
*               | | :  `- \`.;`\ _ /`;.`/ - ` : | |
*               \  \ `-.   \_ __\ /__ _/   .-` /  /
*          ======`-.____`-.___\_____/___.-`____.-'======
*                             `=---='
*          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
*                     佛祖保佑        永无BUG
*            佛曰:
*                   写字楼里写字间，写字间里程序员；
*                   程序人员写程序，又拿程序换酒钱。
*                   酒醒只在网上坐，酒醉还来网下眠；
*                   酒醉酒醒日复日，网上网下年复年。
*                   但愿老死电脑间，不愿鞠躬老板前；
*                   奔驰宝马贵者趣，公交自行程序员。
*                   别人笑我忒疯癫，我笑自己命太贱；
*                   不见满街漂亮妹，哪个归得程序员？

===========================================================</remark>*/
BM2F_ENTERACE(mmsm55a_upd);
int f_mmsm_count(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm55a_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString cs_station_no = "";
	CString cs_bunker_number = "";
	CString cs_mat_simple_ename = "";
	CString cs_factory_div = "";
	CString cs_flag = "";
	CString s_columns = "";
	CString cs_mat_code = "";
	CString cs_mat_name = "";
	CDecimal v_proc_count = 0;
	/* 实体类定义 */
	//CTWMA1 twma1_q(conn);
	//CTWMA1 twma1(conn);


	/* 数据库SQL操作字符串 */
	CString sqlstr = "";
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CModel tmmsm55a("TMMSM55A");
	//系统的分页类信息。
	CPageInfo pageInfo;

	//返回数据信息


	try
	{
		/*int blkNum = bcls_rec->Tables.IndexOf(0);
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 DMCX 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}*/
		/*if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
		{
			cs_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString();
		}
		if (bcls_rec->Tables[0].Columns.Contains("IN_STOCK_NO"))
		{
			cs_bunker_number = bcls_rec->Tables[0].Rows[0]["IN_STOCK_NO"].ToString();
		}
		if (bcls_rec->Tables[0].Columns.Contains("FLAG"))
		{
			cs_flag = bcls_rec->Tables[0].Rows[0]["FLAG"].ToString();
		}*/
		if (bcls_rec->Tables[0].Columns.Contains("IN_STOCK_NO"))
		{
			cs_bunker_number = bcls_rec->Tables[0].Rows[0]["IN_STOCK_NO"].ToString();
		}
		/*if (bcls_rec->Tables[0].Columns.Contains("MAT_NAME"))
		{
			cs_mat_name = bcls_rec->Tables[0].Rows[0]["MAT_NAME"].ToString();
		}
		if (bcls_rec->Tables[0].Columns.Contains("MAT_CODE"))
		{
			cs_mat_code = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString();
		}*/
		/*Log::Trace("", __FUNCTION__, ".[{0}]", bcls_rec->Tables.get_Count());*/
		//Log::Trace("", __FUNCTION__, "..[{0}]", cs_bunker_number);
		//Log::Trace("", __FUNCTION__, "...[{0}]", cs_factory_div);
		//Log::Trace("", __FUNCTION__, ".[{0}]", bcls_rec->Tables[0].Columns.get_Count());
		//Log::Trace("", __FUNCTION__, ".[{0}]", bcls_rec->Tables[0].get_TableName());

		if (bcls_rec->Tables.IndexOf("MMSM55A_INS") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM55A_INS"].Rows.get_Count(); i++)
			{
				
				for (int n = 0; n < bcls_rec->Tables["MMSM55A_INS"].Columns.get_Count(); n++)
				{
					s_columns = bcls_rec->Tables["MMSM55A_INS"].Columns[n].get_ColumnName();

					Log::Trace("", __FUNCTION__, "s_columns=[{0}],columns=[{1}]", s_columns, s_columns.Substring(1, 3));
					
					if (s_columns.Substring(0,2)=="A0")
					{
						
						if (bcls_rec->Tables["MMSM55A_INS"].Rows[i][s_columns].ToDecimal()>0)
						{
							Log::Trace("", __FUNCTION__, "11111");
							tmmsm55a.Reset();
							tmmsm55a.MergeFrom(bcls_rec->Tables["MMSM55A_INS"].Rows[i]);
							//tmmsm55a["IN_STOCK_NO"] = cs_bunker_number;
							//tmmsm55a["FACTORY_DIV"] = cs_factory_div;
							/*tmmsm55a["MAT_NAME"] = cs_mat_name;
							tmmsm55a["MAT_CODE"] = cs_mat_code;*/
							tmmsm55a["REC_CREATOR"] = s.userid;   //记录创建责任者
							tmmsm55a["REC_CREATE_TIME"] = datetime;   //记录创建时刻
							Log::Trace("", __FUNCTION__, "2222");
							tmmsm55a["ELM_NAME"] = s_columns.Substring(1, 3);
							Log::Trace("", __FUNCTION__, "35235");
							tmmsm55a["ELM_VALUE"] = bcls_rec->Tables["MMSM55A_INS"].Rows[i][s_columns].ToDecimal();
							Log::Trace("", __FUNCTION__, "3333");
							tmmsm55a.TrimOrBlank();
							tmmsm55a.Print();
							tmmsm55a.Insert();
						}
						
					}
				}
				/*sqlstr = " SELECT nvl(MAX(PROC_COUNT),0) FROM tmmsm55a  WHERE IN_STOCK_NO= @IN_STOCK_NO AND MAT_CODE=@MAT_CODE ";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("IN_STOCK_NO", tmmsm55a["IN_STOCK_NO"]);
				cmd_inq.Parameters.Set("MAT_CODE", tmmsm55a["MAT_CODE"]);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					v_proc_count = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();
				tmmsm55a["PROC_COUNT"] = v_proc_count + 1;*/

			}

		}

		// 修改事件
		if (bcls_rec->Tables.IndexOf("MMSM55A_UPD") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM55A_UPD"].Rows.get_Count(); i++)
			{

				for (int n = 0; n < bcls_rec->Tables["MMSM55A_UPD"].Columns.get_Count(); n++)
				{
					s_columns = bcls_rec->Tables["MMSM55A_UPD"].Columns[n].get_ColumnName();

					Log::Trace("", __FUNCTION__, "s_columns=[{0}],columns=[{1}]", s_columns, s_columns.Substring(1, 3));

					if (s_columns.Substring(0, 2) == "A0")
					{

						if (bcls_rec->Tables["MMSM55A_UPD"].Rows[i][s_columns].ToDecimal()>0)
						{
							Log::Trace("", __FUNCTION__, "11111");
							tmmsm55a.Reset();
							tmmsm55a.MergeFrom(bcls_rec->Tables["MMSM55A_UPD"].Rows[i]);
							//tmmsm55a["IN_STOCK_NO"] = cs_bunker_number;
							//tmmsm55a["FACTORY_DIV"] = cs_factory_div;
							/*tmmsm55a["MAT_NAME"] = cs_mat_name;
							tmmsm55a["MAT_CODE"] = cs_mat_code;*/
							tmmsm55a["REC_CREATOR"] = s.userid;   //记录创建责任者
							tmmsm55a["REC_CREATE_TIME"] = datetime;   //记录创建时刻
							Log::Trace("", __FUNCTION__, "2222");
							tmmsm55a["ELM_NAME"] = s_columns.Substring(1, 3);
							Log::Trace("", __FUNCTION__, "35235");
							tmmsm55a["ELM_VALUE"] = bcls_rec->Tables["MMSM55A_UPD"].Rows[i][s_columns].ToDecimal();
							Log::Trace("", __FUNCTION__, "3333");
							tmmsm55a.TrimOrBlank();
							tmmsm55a.Print();
							tmmsm55a.Update("ELM_VALUE,REC_REVISOR,REC_REVISE_TIME", "IN_STOCK_NO,MAT_CODE,ELM_NAME");
						}

					}
				}

				//tmmsm55a.Reset();
				//tmmsm55a.MergeFrom(bcls_rec->Tables["MMSM55A_UPD"].Rows[i]);
				//
				///* 修改事件信息 */
				//tmmsm55a["REC_REVISOR"] = s.userid;
				//tmmsm55a["REC_REVISE_TIME"] = datetime;
				//tmmsm55a.TrimOrBlank();
				//tmmsm55a.Update("ELM_CODE,ELM_NAME,ELM_VALUE,REC_REVISOR,REC_REVISE_TIME", "IN_STOCK_NO,MAT_CODE,PROC_COUNT");

			}
		}

		// 删除事件
		if (bcls_rec->Tables.IndexOf("MMSM55A_DEL") >= 0)
		{
			for (int i = 0; i < bcls_rec->Tables["MMSM55A_DEL"].Rows.get_Count(); i++)
			{
				tmmsm55a.Reset();
				tmmsm55a.MergeFrom(bcls_rec->Tables["MMSM55A_DEL"].Rows[i]);
				/*tmmsm55a["IN_STOCK_NO"] = cs_bunker_number;
				tmmsm55a["FACTORY_DIV"] = cs_factory_div;
				tmmsm55a["MAT_NAME"] = cs_mat_name;
				tmmsm55a["MAT_CODE"] = cs_mat_code;*/

				tmmsm55a.TrimOrBlank();
				/* 删除事件信息 */
				tmmsm55a.Delete("IN_STOCK_NO,MAT_CODE");

			}
		}





		if (cs_flag == "I")
		{
			for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
			{
				
			}
		}
		else if (cs_flag == "D")
		{
			for (int i = 0; i < bcls_rec->Tables["DEL"].Rows.get_Count(); i++)
			{
				
			}

		}



		//返回记录总数

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };

		/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006"), arguments, 1);
		CString str = ex.GetMsg() + "\r\n" + sqlstr;

		/*返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应*/
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);

		/*数据库异常时返回-1，事务将被回滚*/
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;

}

