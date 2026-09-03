/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      郑强强
Version:     1.0
Date:        2023-01-12 13:44:44
Description: 单表通用保存-信融专用后台
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(mmsm85_save)


int f_mmsm85_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;

	CString sqlstr = " ";
	CString table_name = " ";
	CString msgstr = "提示信息:";	//提示信息。
	int proc_sum = 0;				//操作总数
	CModel tmmsm85 = CModel("TMMSM85");
	CModel tmmsm85_bak = CModel("TMMSM85_BAK");
	CModel tmmsm60 = CModel("TMMSM60");
	CModel tmmsm50 = CModel("TMMSM50");
	
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_inq(conn);
	CDataTable temp_table;
	CString v_tab = "1";
	try
	{
		//获取传入参数
		CString  nowTime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		table_name = "TMMSM85";
		//bcls_rec->Tables["PARA"].Rows[0]["TABLE_NAME"].ToString();

		//判断是期初是哪个表
		if (bcls_rec->Tables.Contains("TAB"))
		{
			v_tab = bcls_rec->Tables["TAB"].Rows[0]["TAB_FLAG"].ToString();

		}
		if (v_tab == "1")
		{

			if (bcls_rec->Tables.Contains("ADD"))
			{
				Log::Trace("", "", "新增开始,count=[{0}]", bcls_rec->Tables["ADD"].Rows.get_Count());
				for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
				{
					tmmsm85.Reset();
					tmmsm85.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
					tmmsm85_bak.Reset();
					tmmsm85_bak.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
					tmmsm85["REC_CREATOR"] = "QC";
					tmmsm85["REC_CREATE_TIME"] = nowTime;
					tmmsm85["FACTORY_DIV"] = "LG1";
					tmmsm85["RECEIVING_STATUS"] = " ";
					tmmsm85["STATION_NO"] = tmmsm85["STATION_NO"].ToString().SubstringNE(0, 2);
					tmmsm85_bak["REC_CREATOR"] = "QC";
					tmmsm85_bak["REC_CREATE_TIME"] = nowTime;
					tmmsm85_bak["FACTORY_DIV"] = "LG1";
					tmmsm85_bak["RECEIVING_STATUS"] = " ";
					tmmsm85_bak["STATION_NO"] = tmmsm85["STATION_NO"].ToString().SubstringNE(0, 2);
					tmmsm85_bak.TrimOrBlank();
					tmmsm85_bak.Insert();

					if (tmmsm85.Query("MAT_CODE,WEIGH_NO,SEQ_NO,BUNKER_NO"))
					{
						Log::Trace("", "", "新增开始,MAT_CODE=[{0}];WEIGH_NO=[{1}],SEQ_NO=[{2}],BUNKER_NO=[{3}]", tmmsm85["MAT_CODE"].ToString(), tmmsm85["WEIGH_NO"].ToString(), tmmsm85["SEQ_NO"].ToString(), tmmsm85["BUNKER_NO"].ToString());
					}

					tmmsm85.Delete("MAT_CODE,WEIGH_NO,SEQ_NO,BUNKER_NO");

					//Log::Trace("", "", "新增开始,count=[{0}];[{1}]", tmmsm85["WEIGH_NO"].ToString(), i);
					tmmsm85.TrimOrBlank();
					tmmsm85.Insert();
				}
			}

			//更新85表的二级物料编码
			sqlstr = " update TMMSM85 t1 set MAT_CODE_LOT_NO = (select MAT_CODE_L2||'@'||LOT_NO from tmmsm50 t2 where t1.mat_code =t2.mat_code)"
				" where EXISTS(select 1 from tmmsm50 t2 where t1.mat_code = t2.mat_code and LOT_NO != ' ') "
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//更新比例
			sqlstr = "update tmmsm60 t1 set stock_wt = 0"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = "update tmmsm60 t1 set stock_wt = (select sum(stock_wt) from tmmsm85 t2 where t1.bunker_no=t2.bunker_no)"
				"where exists(select 1 from tmmsm85 t2 where t1.bunker_no = t2.bunker_no)"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			//更新容积率
			sqlstr = "update tmmsm60 t1 set RATE = round(STOCK_WT/UPPER_LIMIT_VALUE,3)"
				"where UPPER_LIMIT_VALUE>0"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = "update tmmsm60 t1 set BACK_C1 = '0'"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();

			sqlstr = "update tmmsm60 t1 set BACK_C1 = '1'"
				"where exists(select 1 from tmmsm85 t2 where t1.bunker_no = t2.bunker_no)"
				" and bunker_type in ('EAFBOX','BOFBOX','AODBOX')"
				;
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteNonQuery();
			cmd_inq.Close();
		}
		if (v_tab == "2") //物料编码
		{
			//根据数据进行更新
			if (bcls_rec->Tables.Contains("ADD"))
			{
				for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
				{
					tmmsm50.Reset();
					tmmsm50.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
					tmmsm50["REC_CREATOR"] = "QC";
					tmmsm50["REC_CREATE_TIME"] = nowTime;
					if (tmmsm50.QueryCount("MAT_CODE") != 0) //判断记录是否存在
					{
						tmmsm50.Update("REC_CREATOR,REC_CREATE_TIME,MAT_NAME,MAT_CODE_L2,LOT_NO,BACK_C1,CLASSIFY,MAT_SIMPLE_ENAME,PRIMARY_MAT_UNIT", "MAT_CODE");
					}
					else
					{
						
						tmmsm50.TrimOrBlank();
						tmmsm50.Insert();
					}

					//更新物料类型
					sqlstr = "  update tmmsm50 t1 set MAT_TYPE = (select code from tep0002 t2 where t1.BACK_C1=t2.CODE_DESC_1_CONTENT and code_class = 'MS542N')"
					" where 1=1"
						"and EXISTS(select 1 from tep0002 t2 where t1.BACK_C1 = t2.CODE_DESC_1_CONTENT and  code_class = 'MS542N')"
						;
					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.ExecuteNonQuery();
					cmd_inq.Close();

				}

			}
		}

		if (v_tab == "3") //铁区更新物料
		{
			//根据数据进行更新
			if (bcls_rec->Tables.Contains("ADD"))
			{
				for (int i = 0; i < bcls_rec->Tables["ADD"].Rows.get_Count(); i++)
				{
					if (i == 0)
					{
						sqlstr = "update tmmsm50 set SYSTEM_ID_MAT='C' ";
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.ExecuteNonQuery();
						cmd_inq.Close();

					}
					tmmsm50.Reset();
					tmmsm50.MergeFrom(bcls_rec->Tables["ADD"].Rows[i]);
					tmmsm50["REC_CREATOR"] = "QC";
					tmmsm50["REC_CREATE_TIME"] = nowTime;

					tmmsm50["SYSTEM_ID_MAT"] = "B";
					tmmsm50.Update("REC_CREATOR,REC_CREATE_TIME,SYSTEM_ID_MAT", "MAT_CODE");  					

				}

			}
		}

		if (v_tab == "5") //更新SEQ_NO
		{
			//根据数据进行更新
			if (bcls_rec->Tables.Contains("ADD"))
			{
			        /*上传消耗序列 MMLC_XH
					自循环废钢序列 MMLC_ZXH1
					料槽料篮 MMLC_LS  MMLC_SJ*/
				CDecimal xh_now = 0;
				CDecimal xh1_now = 0;
				CDecimal ls_now = 0;
				CDecimal sj_now = 0;
				xh1_now = bcls_rec->Tables["ADD"].Rows[0]["MMLC_ZXH1"].ToDecimal();
				xh_now = bcls_rec->Tables["ADD"].Rows[0]["MMLC_XH"].ToDecimal();
				ls_now = bcls_rec->Tables["ADD"].Rows[0]["MMLC_LS"].ToDecimal();
				sj_now = bcls_rec->Tables["ADD"].Rows[0]["MMLC_SJ"].ToDecimal();
				CDecimal xh_seq = 0;
				sqlstr = "  select MMLC_ZXH1.nextval from dual"
					;
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					xh_seq = cmd_inq.GetDecimal(1);
				}
				cmd_inq.Close();

				Log::Trace("", "", "xh_seq=[{0}]", xh_seq);
				xh_seq = xh1_now - xh_seq;
				Log::Trace("", "", "xh_seq1=[{0}]", xh_seq);
			//	sqlstr = "  alter SEQUENCE MMLC_ZXH1 increment by  " + xh_seq.ToString() ;
			//	Log::Trace("", "", "sqlstr=[{0}]", sqlstr);
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.ExecuteNonQuery();				
			//	cmd_inq.Close();
			//		tpcommit(0);
			//	tpbegin(0, 0);
			//	

			//	sqlstr = " select MMLC_ZXH1.nextval from dual  "
			//		;
			//	Log::Trace("", "", "sqlstr2=[{0}]", sqlstr);
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.ExecuteNonQuery();
			//	cmd_inq.Close();
			//	tpcommit(0);
			//	tpbegin(0, 0);

			///*	conn->Commit();
			//	conn->BeginTransaction(); */ 		

			///*	tpcommit(0);
			//	tpbegin(0, 0);*/

			//	sqlstr = " alter SEQUENCE MMLC_ZXH1 increment by 1"
			//		;
			//	cmd_inq.SetCommandText(sqlstr);
			//	cmd_inq.ExecuteNonQuery();
			//	cmd_inq.Close();
			//	/*conn->Commit();
			//	conn->BeginTransaction();*/
			//	cmd_inq.Close();
				//单步提交数据库

					/*alter SEQUENCE MMLC_ZXH1 increment by 654;
				select MMLC_ZXH1.nextval from dual;
				;

				alter SEQUENCE MMLC_XH increment by 654;
				select MMLC_XH.nextval from dual;
				alter SEQUENCE MMLC_XH increment by 1;

				alter SEQUENCE MMLC_LS increment by 654;
				select MMLC_LS.nextval from dual;
				alter SEQUENCE MMLC_LS increment by 1;

				alter SEQUENCE MMLC_SJ increment by 654;
				select MMLC_SJ.nextval from dual;
				alter SEQUENCE MMLC_SJ increment by 1; "*/

			}
		}
		

		msgstr += msgstr.Format("%d条记录操作成功。", proc_sum);
		strncpy(s.msg, (const char*)msgstr, sizeof(s.msg) - 1);

	}
	catch (CDbException& ex)  //捕获数据库操作异常 
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。", arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
		////Log::Warn("", __FUNCTION__, "CDbException: {0}", s.msg);
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
		////Log::Error("", __FUNCTION__, "CApplicationException: {0}", ex.GetMsg());
	}
	catch (CException& ex)
	{
		strncpy(s.sysmsg, (const char*)ex.GetMsg(), sizeof(s.sysmsg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
		////Log::Fatal("", __FUNCTION__, "CException: {0}", ex.GetMsg());
	}
	return doFlag;
}


