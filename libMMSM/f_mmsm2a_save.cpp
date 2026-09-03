
#include "stdafx.h"
 

BM2_FUNCTION_EXPORT
//外部函数声明
int f_mmsm2a_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_confirm_flag(const CString& factory_div, const CString& heat_no, CString& heat_confirm_flag, CDbConnection * conn);
int f_mmsm_acyfl(EIClass * bcls_rec, CString & flag, EIClass * bcls_ret, CString& v_acjc_relation_id, CDbConnection * conn);
int f_mmsm_acyfl_seq(CString& v_acjc_relation_id, CDbConnection * conn);

int f_mmsm2a_save(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;


	/* 业务变量 */
	CString v_area_id = "";
	CString v_heat_confirm_flag = "";
	CString v_acjc_flag = "";
	CString v_acjc_relation_id = "";
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CString sqlstr_temp;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	/* 实体类定义 */
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm2a_new("TMMSM2A");
	CModel tmmsm2a_old("TMMSM2A");

	try
	{
		//获取消耗主键信息
		tmmsm2a.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		tmmsm2a.Print();

		//已炉次确定则返回
		doFlag = f_mmsm_confirm_flag(tmmsm2a["FACTORY_DIV"].ToString(), tmmsm2a["HEAT_NO"].ToString(), v_heat_confirm_flag, conn);

		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (v_heat_confirm_flag != "0" && v_heat_confirm_flag != "")
		{
			sprintf(s.msg, "该制造命令号已经炉次确定"); //系统错误信息
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//设置MMSM2A表为最新消耗单元
		bcls_rec->Tables[0].set_TableName("MMSM2A_NEW");
		//tmmsm2a.MergeFrom(bcls_rec->Tables["MMSM2A_NEW"].Rows[0]);

		//设置MMSM2A_OLD表为旧消耗单元
		if (!bcls_rec->Tables.Contains("MMSM2A_OLD"))
		{
			bcls_rec->Tables.Add("MMSM2A_OLD");
			bcls_rec->Tables["MMSM2A_OLD"].Columns.Add(tmmsm2a);
		}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr = "SELECT TMMSM2A.*,' ' PROC_DIV,0 NEW_WT,' ' NEW_HANDWORK_MARK,' ' NEW_COLL_MODE FROM TMMSM2A WHERE PROC_NO = @tmmsm2a.PROC_NO";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("tmmsm2a.PROC_NO", tmmsm2a["PROC_NO"].ToString());
		cmd_inq.ExecuteQuery(bcls_rec->Tables["MMSM2A_OLD"]);



		//数据对比,结果保存在MMSM2A_OLD表
		for (int i = 0; i < bcls_rec->Tables["MMSM2A_NEW"].Rows.get_Count(); i++)
		{
			tmmsm2a_new.Reset();
			tmmsm2a_new.MergeFrom(bcls_rec->Tables["MMSM2A_NEW"].Rows[i]);

			//Log::Trace("", "", "NEW DATA[{2}]== tmmsm2a["MAT_CODE"] ={0}, tmmsm2a["DEVO_WT"] ={1}", tmmsm2a["MAT_CODE"].ToString(), tmmsm2a["DEVO_WT"].ToDecimal(), i);

			CString ifExist = "0";

			for (int j = 0; j < bcls_rec->Tables["MMSM2A_OLD"].Rows.get_Count(); j++)
			{
				tmmsm2a_old.Reset();
				tmmsm2a_old.MergeFrom(bcls_rec->Tables["MMSM2A_OLD"].Rows[j]);
				//Log::Trace("", "", "OLD DATA[{2}]== tmmsm2a_old["MAT_CODE"] ={0}, tmmsm2a_old["DEVO_WT"] = {1}", tmmsm2a_old["MAT_CODE"].ToString(), tmmsm2a_old["DEVO_WT"].ToDecimal(), j);

				if (tmmsm2a_new["MAT_CODE"].ToString() == tmmsm2a_old["MAT_CODE"].ToString())
				{
					bcls_rec->Tables["MMSM2A_OLD"].Rows[j]["PROC_DIV"] = "U";
					bcls_rec->Tables["MMSM2A_OLD"].Rows[j]["NEW_WT"] = tmmsm2a_new["DEVO_WT"];
					bcls_rec->Tables["MMSM2A_OLD"].Rows[j]["NEW_HANDWORK_MARK"] = tmmsm2a_new["HANDWORK_MARK"];
					bcls_rec->Tables["MMSM2A_OLD"].Rows[j]["NEW_COLL_MODE"] = tmmsm2a_new["PRACT_COLL_MODE"];
					ifExist = "1";
					break;
				}
			}

			if (ifExist == "0")
			{
				CDataRow & newRow = bcls_rec->Tables["MMSM2A_OLD"].Rows.Add();
				newRow["FACTORY_DIV"] = tmmsm2a["FACTORY_DIV"];
				newRow["PRACT_COLL_MODE"] = tmmsm2a_new["PRACT_COLL_MODE"];
				newRow["HEAT_NO"] = tmmsm2a["HEAT_NO"];
				newRow["PROC_NO"] = tmmsm2a["PROC_NO"];
				newRow["STATION_ID"] = tmmsm2a["STATION_ID"];
				newRow["STATION_NO"] = tmmsm2a["STATION_NO"];
				newRow["MAT_CODE"] = tmmsm2a_new["MAT_CODE"];
				newRow["MAT_NAME"] = tmmsm2a_new["MAT_NAME"];
				newRow["DEVO_WT"] = tmmsm2a_new["DEVO_WT"];
				newRow["HANDWORK_MARK"] = tmmsm2a_new["HANDWORK_MARK"];
				/*newRow["PROD_SHIFT_GROUP"] = tmmsm2a.PROD_SHIFT_GROUP;
				newRow["PROD_SHIFT_NO"] = tmmsm2a.PROD_SHIFT_NO;*/
				newRow["PROC_DIV"] = "I";
			}
		}

		blkNum = bcls_rec->Tables.IndexOf("MMSM2A");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSM2A");
			bcls_rec->Tables["MMSM2A"].Columns.Add(tmmsm2a);
			bcls_rec->Tables["MMSM2A"].Columns.Add(DT_STRING, "PROC_DIV");
			bcls_rec->Tables["MMSM2A"].Columns.Add(DT_DECIMAL, "NEW_WT");
			bcls_rec->Tables["MMSM2A"].Columns.Add(DT_STRING, "NEW_HANDWORK_MARK");
			bcls_rec->Tables["MMSM2A"].Columns.Add(DT_STRING, "NEW_COLL_MODE");
			bcls_rec->Tables["MMSM2A"].Rows.Add();
		}

		//设置抛帐关联关系号ACJC_RELATION_ID
		if (!bcls_rec->Tables["MMSM2A"].Columns.Contains("ACJC_RELATION_ID"))
		{
			bcls_rec->Tables["MMSM2A"].Columns.Add(DT_STRING, "ACJC_RELATION_ID");
		}

		f_mmsm_acyfl_seq(v_acjc_relation_id, conn);
		//处理负数抛帐
		v_acjc_flag = "0";
		//doFlag = f_mmsm_acyfl(bcls_rec, v_acjc_flag, bcls_ret, v_acjc_relation_id, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//循环处理得到结果
		for (int j = 0; j < bcls_rec->Tables["MMSM2A_OLD"].Rows.get_Count(); j++)
		{
			CString matCode = bcls_rec->Tables["MMSM2A_OLD"].Rows[j]["MAT_CODE"].ToString();
			CString procDiv = bcls_rec->Tables["MMSM2A_OLD"].Rows[j]["PROC_DIV"].ToString();
			CDecimal devoWt = bcls_rec->Tables["MMSM2A_OLD"].Rows[j]["DEVO_WT"].ToDecimal();
			CDecimal newWt = bcls_rec->Tables["MMSM2A_OLD"].Rows[j]["NEW_WT"].ToDecimal();
			CString HandWorkMark = bcls_rec->Tables["MMSM2A_OLD"].Rows[j]["HANDWORK_MARK"].ToString();
			CString newHandWorkMark = bcls_rec->Tables["MMSM2A_OLD"].Rows[j]["NEW_HANDWORK_MARK"].ToString();
			CString collMode = bcls_rec->Tables["MMSM2A_OLD"].Rows[j]["PRACT_COLL_MODE"].ToString();
			CString newCollMode = bcls_rec->Tables["MMSM2A_OLD"].Rows[j]["NEW_COLL_MODE"].ToString();

			//Log::Trace("", "", "procDiv = {0}", procDiv);
			//Log::Trace("", "", "PROC DATA[{2}]== matCode = {0}, devoWt = {1},NEW_WT = {3}", matCode, devoWt, j, newWt);
			//Log::Trace("", "", "PROC DATA[{2}]== HandWorkMark = {0}, newHandWorkMark = {1}", HandWorkMark, newHandWorkMark, j);
			//Log::Trace("", "", "PROC DATA[{2}]== collMode = {0}, newCollMode = {1}", collMode, newCollMode, j);


			bcls_rec->Tables["MMSM2A"].Rows[0].Merge(bcls_rec->Tables["MMSM2A_OLD"].Rows[j]);
			if (bcls_rec->Tables["MMSM2A"].Rows[0]["PROC_DIV"].ToString().Trim() == "")
			{
				bcls_rec->Tables["MMSM2A"].Rows[0]["PROC_DIV"] = "D";
			}

			if (procDiv == "U" && devoWt == newWt && HandWorkMark == newHandWorkMark && collMode == newCollMode)
			{
				continue;
			}

			doFlag = f_mmsm2a_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}

		//处理正数抛帐
		v_acjc_flag = "1";
		//doFlag = f_mmsm_acyfl(bcls_rec, v_acjc_flag, bcls_ret, v_acjc_relation_id, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


