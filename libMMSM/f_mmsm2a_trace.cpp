/*************************************************************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   dongcuilian
Version:    1.0
Date:     2015-11-10
Description:	 写原辅料消耗履历表。
Update:
**************************************************************************************************************/

//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件



#if defined _SYS_PES || defined _SYS_MES


#endif
/*<remark>=========================================================
/// <summary>
/// 写炼钢调整履历表
/// <para>处理内容：写炼钢计划履历表履历表  </para>
/// <para>数据库表：tmmsm52 </para>
/// <para>主调用函数：被f_pssm11_ins_heat()、pssm18_chg()等调用。           </para>
/// </summary>
/// <param name="FACTORY_DIV">厂别区分         </param>
/// <param name="pono">制造命令          </param>
/// <param name="heat_no">熔炼号          </param>
/// <returns>无</returns>
===========================================================</remark>*/

BM2_FUNCTION_EXPORT
int f_mmsm2a_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int fetchRowCount;
	int i;
	int rows = 0;
	CString	create_time;            /* 记录创建时刻 */
	CString maxseq = "";
	CString newSeqNo = "";
	int blkseq = 0;

	CModel tep0002("TEP0002");//小代码描述
#if defined _SYS_PES || defined _SYS_MES
	CModel tmmsm2a("TMMSM2A");	
	CModel tmmsm52("TMMSM52");
#endif

	CDbCommand cmd_inq(conn);
	CString sqlstr;


	try
	{
		create_time = CDateTime::Now().ToString("yyyyMMddHHmmss");

		blkseq = bcls_rec->Tables.IndexOf("TRACE");
		if (blkseq < 0)
		{
			sprintf(s.msg, "没有找到传入数据块[TRACE]，请联系系统维护人员。");
			sprintf(s.sysmsg, "TABLE [TRACE] NOT EXIST ");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		/* 对输入信息循环处理 */
		rows = bcls_rec->Tables[0].Rows.get_Count();
		for (i = 1; i <= rows; i++)
		{
			/* 取得单行传入信息 */
			tmmsm52.MergeFrom(bcls_rec->Tables[blkseq].Rows[i - 1]);
			if (tmmsm52["PROC_NO"].ToString().Trim() == "") //不能写入
			{
				CFormattable arguments[] = { tmmsm52["PROC_NO"].ToString() }; // 定义参数列表的数组
				CMessageFormat::Format(s.msg, "要写入履历的炉次HEAT_NO[{0}]为空，不能记录履历。", arguments, 1);
				throw CApplicationException(-1, s.msg, log.Location);
			}


			//获取系统时间和操作人员			
			tmmsm52["REC_CREATOR"] = s.userid;
			tmmsm52["REC_CREATE_TIME"] = create_time;
			tmmsm52["LOG_TIME"] = create_time;
			tmmsm52["REC_REVISOR"] = s.userid;
			tmmsm52["REC_REVISE_TIME"] = create_time;
			tmmsm52["LOG_IP_ADDRESS"] = s.fore_ip;//获取电脑IP
			tmmsm52["LOG_FORMNAME"] = s.formname;
			tmmsm52["LOG_USERNAME"] = s.username;

			tmmsm2a["PROC_NO"] = tmmsm52["PROC_NO"].ToString().Trim();
			tmmsm2a["PROC_COUNT"] = tmmsm52["PROC_COUNT"].ToString().Trim();
			tmmsm2a.Query("PROC_NO,PROC_COUNT");


			tmmsm52["MAT_CODE"] = tmmsm2a["MAT_CODE"];
			tmmsm52["MAT_NAME"] = tmmsm2a["MAT_NAME"];
			//tmmsm52["MAT_TYPE"] = tmmsm2a["MAT_TYPE"];
			tmmsm52["STATION_ID"] = tmmsm2a["STATION_ID"];
			tmmsm52["STATION_NO"] = tmmsm2a["STATION_NO"];
			tmmsm52["DEVO_WT"] = tmmsm2a["DEVO_WT"];
			tmmsm52["DEVO_TIME"] = tmmsm2a["DEVO_TIME"];
			//tmmsm52["DEVO_JOB_POINT"] = tmmsm2a["DEVO_JOB_POINT"]; //投料作业点
			//tmmsm52["PROD_SHIFT_NO"] = tmmsm2a["PROD_SHIFT_NO"];
			//tmmsm52["PROD_SHIFT_GROUP"] = tmmsm2a["PROD_SHIFT_GROUP"];
			//设置序列号
			maxseq = EPGetNextSeq("MMSM52_SEQ", conn);
			newSeqNo = newSeqNo.Format("%4s", (const char*)maxseq);
			Log::Trace("", __FUNCTION__, "newSeqNo=[{0}]", newSeqNo);
			Log::Trace("", __FUNCTION__, "maxseq=[{0}]", maxseq);
			tmmsm52["RESUME_SEQ_NO"] = create_time + newSeqNo;

			tmmsm52.TrimOrBlank();
			tmmsm52.Print();

			////Log::Trace("", __FUNCTION__, "tmmsm52.Insert():PROD_SEQ_NO=[{0}], FACTORY_DIV=[{1}], SM_PLAN_NO=[{2}], PONO=[{3}], EVENT_ID=[{4}], EVENT_DESC=[{5}],INSERT_FALG = [{6}]", tmmsm52["PROD_SEQ_NO"].ToString(), tmmsm52["FACTORY_DIV"].ToString(), tmmsm52["SM_PLAN_NO"].ToString(), tmmsm52["PONO"].ToString(), tmmsm52["EVENT_ID"].ToString(), tmmsm52["EVENT_DESC"].ToString(), tep0002["CODE_DESC_2_CONTENT"].ToString());
			sqlstr = "tmmsm52.Insert()";
			tmmsm52.Insert();

		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		////Log::Trace("", __FUNCTION__, "tmmsm52--ex.GetCode()=[{0}]", ex.GetCode());
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		////Log::Trace("", __FUNCTION__, "s.flag=[{0}],doFlag=[{1}]", s.flag, doFlag);
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

