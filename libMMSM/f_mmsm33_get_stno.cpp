/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-07-04
Description: 物料表信息修改
***********************************************************************/

#include "stdafx.h"


//炉次判定物料事件
int f_mmsm99(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);
#if defined _SYS_PES    //PES层部署时调用的函数
//int f_qm200002_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);  //调用炉次判定接口
#endif

/* =========================================================================
/// <remark>
/// <summary>
/// 接收铸坯产出实绩，新建材料主档（不包括写合同信息、写履历以及状态确定）
/// <para>根据产出铸坯，建立材料主档信息。</para>
/// <para>当同一实物材料分多次报送，不走本函数，走修改分支。  </para>
/// </summary>
/// <param name= "xxxx">输入参数</param>
/// <returns>成功:0</returns>
/// <returns>失败:-1</returns>
/// </remark>
========================================================================= */
BM2_FUNCTION_EXPORT
 int f_mmsm33_get_stno(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int ret = 0;
	int i;
	int blkNum = 0;
	int fetchRowCount;
	CString datetime = "";
	int v_flag = 1;
	CString sqlstr = "";
	CString v_heat_no = "";
	CString v_pono, v_st_no, v_fin_st_no, v_judge_code, v_judge_time;
	EIClass inBlock;          //材料主档信息处理用
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CString MMSMBlock = "MM0099";//约定的块名
	EIClass bcls_rec_sm;
	EIClass bcls_ret_sm;
	//调用物料电文函数，置板坯决定/最终出钢记号、对板坯做封锁、释放
	EIClass bcls_rec_200002;
	EIClass bcls_ret_200002;

	try
	{
		bcls_rec_200002.Tables.Add("MMSMSM");
		bcls_rec_200002.Tables["MMSMSM"].Rows.Clear();

		bcls_rec_sm.Tables[0].set_TableName(MMSMBlock);
		bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "MAT_KIND");		/*物料类型*/
		bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "EVENT_ID");		/*事件标识*/
		bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");	/*事件产线类型*/
		bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "SYSTEM_ID");		/*系统标识*/
		bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "FUNC_ID");			/*功能标识*/
		bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "MAT_NO");			/*功能标识*/
		bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "FIN_ST_NO");			/*功能标识*/
		bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "ST_NO");			/*功能标识*/
		bcls_rec_sm.Tables[MMSMBlock].Columns.Add(DT_STRING, "EVENT_DESC");
		//bcls_rec_sm.Tables[0].Rows.Clear();
		bcls_rec_sm.Tables[MMSMBlock].Rows.Clear();

		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count();i++)
		{
			if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			{
				v_heat_no = bcls_rec->Tables[0].Rows[i]["HEAT_NO"];
			}
			else
			{
				strcpy(s.msg, "熔炼号不能为空!"); 
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			{
				tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"];
			}
			else
			{
				strcpy(s.msg, "材料号不能为空!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01.Query("MAT_NO") == false)
			{
				sprintf(s.msg, "输入材料号[{0}]不存在。", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			inBlock.Tables[0].Clear();
			//1.获取HEAT_NO的炉次判定结果
			sqlstr = CString(
				" SELECT PONO,ST_NO,FIN_ST_NO,JUDGE_CODE,JUDGE_TIME FROM TQMTS23 "
				" WHERE heat_no = @heat_no "
				);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", v_heat_no);
			int t1 = cmd_inq.ExecuteQuery(inBlock.Tables[0]);
			//Log::Trace("", __FUNCTION__, "row num=[{0}]", t1);
			if (t1 == 1)
			{
				v_pono = inBlock.Tables[0].Rows[0]["PONO"];
				v_st_no = inBlock.Tables[0].Rows[0]["ST_NO"];
				v_fin_st_no = inBlock.Tables[0].Rows[0]["FIN_ST_NO"];
				v_judge_code = inBlock.Tables[0].Rows[0]["JUDGE_CODE"];
				v_judge_time = inBlock.Tables[0].Rows[0]["JUDGE_TIME"];

				bcls_rec_sm.Tables[0].Rows.Add();
				int mat_count = bcls_rec_sm.Tables[0].Rows.get_Count()-1;
				//Log::Trace("", __FUNCTION__, "mat_count=[{0}]", mat_count);

				//调炼钢物料函数用	
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["MAT_KIND"] = tmmsm01["MAT_KIND"];
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["EVENT_ID"] = "QM70";
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["EVENT_LINE_TYPE"] = "00";
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["SYSTEM_ID"] = "QMTS";
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["MAT_NO"] = tmmsm01["MAT_NO"];
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["FUNC_ID"] = s.svc_name;
				bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["EVENT_DESC"] = "炉次钢种判定_炉次判定后切割坯自动补判信息";
				if (v_judge_code.Trim() == "1")
				{
					v_flag = 1;
					bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["ST_NO"] = v_fin_st_no;
					bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["FIN_ST_NO"] = v_fin_st_no;
				}
				else if (v_judge_code.Trim() == "2")
				{
					bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["ST_NO"] = v_st_no;
					bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]["FIN_ST_NO"] = v_fin_st_no;
					v_flag = 1;
				}
				else
				{
					v_flag = 0;
				}
				if (v_flag == 1)
				{
					tmmsm96.Reset();
					tmmsm96.MergeFrom(bcls_rec_sm.Tables[MMSMBlock].Rows[mat_count]);
					//HYF 20130502 这里的bcls_rec_200002　是反复累加的，导致多发电文
					tmmsm96.MergeTo(bcls_rec_200002.Tables["MMSMSM"], false);
				}
				

			}
			else
			{
				v_flag = 0;
			}
			
		}
		if (v_flag == 1)
		{
			doFlag = f_mmsm99(&bcls_rec_sm, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			

			//炉次确定时下达更新板坯的出钢记号电文
			#if defined _SYS_PES
			/*doFlag = f_qm200002_snd(&bcls_rec_200002, &bcls_ret_200002, conn);
			if (doFlag != 0)
			{
				Log::Trace("", __FUNCTION__, "f_qm200002_snd() msg = [{0}]", s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}*/
			#endif
		}
		
		
	


	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;

}


