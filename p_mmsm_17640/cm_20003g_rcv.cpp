/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     向萍
Version:    1.0
Date:       2014-08-24
Description: 炼钢材料拆批电文接收
**************************************************/
//框架头文件
#include "stdafx.h" 
#include "epex.h" 

/*<remark>=========================================================
/// <summary>
/// 棒线钢坯拆批电文接收
/// <para>
/// * 根据传入的母材料号和拆批材料号,调用拆批函数。
/// <para>
/// </summary>
/// <param name="MAT_NO">材料号 </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件



//外部函数声明
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) ;

// service入口
BM2F_ENTERACE_TELE(cm_20003g_rcv)

int f_cm_20003g_rcv(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");    

	/* 实体类定义 */
	CModel tmmsm96("TMMSM96");
	CModel tmmsm01("TMMSM01");
	CModel new_tmmsm01("TMMSM01");
	CModel para_tmmsm01("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

  try
  { 
	  datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	  /* 添加并设置块名 */
	  blkNum = bcls_rec->Tables.IndexOf("MM0099");
	  if (blkNum < 0)
	  {
		  bcls_rec->Tables.Add("MM0099");
	  }

	  /* 获取输入参数 */
	  tmmsm96.MergeFrom(bcls_rec->Tables[0].Rows[0]);		//母材料信息
	  para_tmmsm01.MergeFrom(bcls_rec->Tables[0].Rows[1]);//拆批产生的新材料信息

	  ////Log::Info("", __FUNCTION__, "tmmsm96.EVENT_ID		= [{0}]", tmmsm96["EVENT_ID"].ToString());			//调用事件号
	  ////Log::Info("", __FUNCTION__, "tmmsm96.MAT_NO		= [{0}]", tmmsm96["MAT_NO"].ToString());				//母材料号
	  ////Log::Info("", __FUNCTION__, "para_tmmsm01.MAT_NO	= [{0}]", para_tmmsm01["MAT_NO"].ToString());			//拆批产生的新材料号
	  ////Log::Info("", __FUNCTION__, "tmmsm96.MAT_NUM		= [{0}]", tmmsm96["MAT_NUM"].ToDecimal());				//拆批根数


	  /* 检查输入参数合法性 */
	  if (tmmsm96["MAT_NO"].ToString().Trim() == "")
	  {
		  sprintf(s.msg, "材料号不能为空");
		  throw CApplicationException(-1, s.msg, log.Location);
	  }

	  /* 查询母材料板坯主档信息 */
	  tmmsm01["MAT_NO"] = tmmsm96["MAT_NO"];
	  tmmsm01.Query("MAT_NO");

	  /* 设置新材料主档信息 */
	  new_tmmsm01.CopyFrom(tmmsm01);

	  /* 按传入材料信息新增材料档 */
	  new_tmmsm01["MAT_NO"] = para_tmmsm01["MAT_NO"];
	  new_tmmsm01["MAT_NO_OLD"] = para_tmmsm01["MAT_NO_OLD"];
	  ////母材料号为编入计划状态,新材料的计划号清空
	  //if(tmmsm01["PLAN_NO"].ToString().Trim() != "")
	  //{
	  //	new_tmmsm01.PLAN_NO	= "";
	  //}
	  new_tmmsm01["MAT_NUM"] = para_tmmsm01["MAT_NUM"];
	  new_tmmsm01["MAT_NUM_CONFM"] = para_tmmsm01["MAT_NUM_CONFM"];
	  new_tmmsm01["MAT_ACT_WT"] = para_tmmsm01["MAT_ACT_WT"];
	  new_tmmsm01["MAT_THEORY_WT"] = para_tmmsm01["MAT_THEORY_WT"];
	  if (new_tmmsm01["MEASURE_WT_FLAG"].ToString().Trim() == "0") //0-未称重
	  {
		  new_tmmsm01["MAT_WT"] = new_tmmsm01["MAT_THEORY_WT"];
	  }
	  else if (new_tmmsm01["MEASURE_WT_FLAG"].ToString().Trim() == "1") //1-已称重
	  {
		  new_tmmsm01["MAT_WT"] = new_tmmsm01["MAT_ACT_WT"];
	  }
	  new_tmmsm01["STOCK_OPER_ORDER"] = para_tmmsm01["STOCK_OPER_ORDER"];
	  new_tmmsm01["IN_FLAG"] = para_tmmsm01["IN_FLAG"];
	  new_tmmsm01["STOCK_PLACE_NO"] = para_tmmsm01["STOCK_PLACE_NO"];
	  new_tmmsm01["STOCK_NO"] = para_tmmsm01["STOCK_NO"];
	  new_tmmsm01["REC_CREATOR"] = para_tmmsm01["REC_CREATOR"];
	  new_tmmsm01["REC_CREATE_TIME"] = para_tmmsm01["REC_CREATE_TIME"];
	  new_tmmsm01["REC_REVISOR"] = para_tmmsm01["REC_REVISOR"];
	  new_tmmsm01["REC_REVISE_TIME"] = para_tmmsm01["REC_REVISE_TIME"];
	  new_tmmsm01.TrimOrBlank();
	  new_tmmsm01.Insert();

	  /* 记录拆批履历 */
	  bcls_rec->Tables["MM0099"].Clear();
	  bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
	  bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
	  bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
	  bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
	  bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
	  bcls_rec->Tables["MM0099"].Rows.Add();
	  bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM3A";	//材料为拆批产生
	  bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
	  bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
	  bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "cm_20003g_rcv";
	  bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = new_tmmsm01["MAT_NO"];
	  doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
	  if (doFlag < 0)
	  {
		  throw CApplicationException(-1, s.msg, s.svc_name);
	  }

	  /* 更新母材料信息 */
	  //tmmsm01["MAT_NO"] = tmmsm96["MAT_NO"];
	  //tmmsm01["MAT_NUM"] = tmmsm96["MAT_NUM"];
	  //tmmsm01["MAT_NUM_CONFM"] = tmmsm96["MAT_NUM_CONFM"];
	  //tmmsm01["MAT_ACT_WT"] = tmmsm96["MAT_ACT_WT"];
	  //tmmsm01["MAT_THEORY_WT"] = tmmsm96["MAT_THEORY_WT"];
	  //if (tmmsm01["MEASURE_WT_FLAG"].ToString().Trim() == "0") //0-未称重
	  //{
		 // tmmsm01["MAT_WT"] = tmmsm01["MAT_THEORY_WT"];
	  //}
	  //else if (new_tmmsm01["MEASURE_WT_FLAG"].ToString().Trim() == "1") //1-已称重
	  //{
		 // tmmsm01["MAT_WT"] = tmmsm01["MAT_ACT_WT"];
	  //}
	  //tmmsm01["REC_REVISOR"] = s.userid;
	  //tmmsm01["REC_REVISE_TIME"] = datetime;
	  //tmmsm01.Update("MAT_NUM,"
			//		 "MAT_NUM_CONFM,"
			//		 "MAT_ACT_WT,"
			//		 "MAT_THEORY_WT,"
			//		 "REC_REVISOR,"
			//		 "REC_REVISE_TIME", "MAT_NO");

	  /* 设置物料跟踪参数 */
	  bcls_rec->Tables["MM0099"].Clear();
	  bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
	  bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
	  bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
	  bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
	  bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
	  bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NUM");
	  bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_WT");
	  bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_ACT_WT");
	  bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_THEORY_WT");

	  bcls_rec->Tables["MM0099"].Rows.Add();
	  bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = tmmsm96["EVENT_ID"];
	  bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = tmmsm96["EVENT_LINE_TYPE"];
	  bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = tmmsm96["SYSTEM_ID"];
	  bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = tmmsm96["FUNC_ID"];
	  bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm96["MAT_NO"];
	  bcls_rec->Tables["MM0099"].Rows[0]["MAT_NUM"] = tmmsm96["MAT_NUM"];
	  bcls_rec->Tables["MM0099"].Rows[0]["MAT_WT"] = tmmsm96["MAT_WT"];
	  bcls_rec->Tables["MM0099"].Rows[0]["MAT_ACT_WT"] = tmmsm96["MAT_ACT_WT"];
	  bcls_rec->Tables["MM0099"].Rows[0]["MAT_THEORY_WT"] = tmmsm96["MAT_THEORY_WT"];
	  doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
	  if (doFlag < 0)
	  {
		  throw CApplicationException(-1, s.msg, s.svc_name);
	  }
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,"数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		//Log::Trace((1,1, "[%s]", s.sysmsg);
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
