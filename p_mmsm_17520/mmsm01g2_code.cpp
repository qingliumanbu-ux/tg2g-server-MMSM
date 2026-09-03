/*<remark>=========================================================
/// <summary>
/// 获取TMMSM01和HMMSM01的实际全程工序途径码作为下拉源
///或者取设置的值集代码
/// </summary>
///
/// <returns>满足查询条件的值集代码数据</returns>
===========================================================</remark>*/
#include "stdafx.h"
BM2F_ENTERACE(mmsm01g2_code)

int f_mmsm01g2_code(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int sql_flag = 0;
	int i = 0;

	CString sqlstr = " ";
	int	blkNum1 = 0;
	int	blkNum = 0;
	/* 实体类定义 */
	try
	{
		CDbCommand cmdcount(conn);
		CDbCommand cmdinq(conn);

		CString strSql = "";
		CString v_code_class = bcls_rec->Tables[0].Rows[0]["code_class"];

		/*查询炉次命令信息 传入块1*/
		bcls_ret->Tables[0].set_TableName("EP0002");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "CODE");
		bcls_ret->Tables[0].Columns.Add(DT_STRING, "CODE_DESC_1_CONTENT");

		/*数据校验*/
		if (v_code_class.Trim().Compare(" ") <= 0)
		{
			throw CApplicationException(_RES("MMSMS0000228")/*cast_lot_no不能为空*/);
			Log::Trace("", __FUNCTION__, "v_code_class 【{0}】不能为空", (const char*)v_code_class);
		}

		if (v_code_class.Trim() == "0")
		{

			sprintf(s.msg, "仅有薄板");
			throw CApplicationException(-1, s.msg, log.Location);

			/*查询SIPM.TSIPMOF02*/
			/*strSql = "SELECT WHOLE_BACKLOG_CODE as 全程工序代码,WHOLE_BACKLOG_NAME as 全程工序名称,UNIT_CODE as 机组代码,BACKLOG_POS as 工序位,"
			"BACKLOG_VAL as 工序值 FROM SIPM.TSIPMOF02 ORDER BY BACKLOG_POS ASC";*/
			
			strSql = "SELECT WHOLE_BACKLOG_CODE,WHOLE_BACKLOG_NAME ,UNIT_CODE ,BACKLOG_POS ,"
				"BACKLOG_VAL  FROM SIPM.TSIPMOF02 ORDER BY BACKLOG_POS ASC";
			cmdinq.SetCommandText(strSql);
			cmdinq.Prepare();
			Log::Trace("", __FUNCTION__, "strSql = 【{0}】", (const char*)strSql);
			cmdinq.ExecuteQuery(bcls_ret->Tables["EP0002"]);
			bcls_ret->Tables["EP0002"].Rows.Add();



		}
		else if (v_code_class.Trim() == "1")
		{
			/*查询SIPM.TSIPMOF01*/
			/*strSql = "SELECT WHOLE_BACKLOG_CODE ,WHOLE_BACKLOG_NAME ,UNIT_CODE ,BACKLOG_POS ,"
				"BACKLOG_VAL  FROM SIPM.TSIPMOF01 ORDER BY BACKLOG_POS ASC";*/

			strSql = "SELECT  DISTINCT WHOLE_BACKLOG_CODE AS 全程工序代码 FROM TMMSM01"
				" UNION SELECT  DISTINCT WHOLE_BACKLOG_CODE AS 全程工序代码 FROM HMMSM01";
			cmdinq.SetCommandText(strSql);
			cmdinq.Prepare();
			Log::Trace("", __FUNCTION__, "strSql = 【{0}】", (const char*)strSql);
			cmdinq.ExecuteQuery(bcls_ret->Tables["EP0002"]);
			/*bcls_ret->Tables["EP0002"].Rows.Add();*/
		}

		else
		{
			/*根据标记查询在线档或历史档  分页*/

			strSql = " SELECT DISTINCT CODE,CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = @code_class ORDER BY CODE asc";


			cmdinq.SetCommandText(strSql);
			cmdinq.Prepare();
			cmdinq.Parameters.Set("code_class", v_code_class);
			Log::Trace("", __FUNCTION__, "strSql = 【{0}】", (const char*)strSql);
			cmdinq.ExecuteQuery(bcls_ret->Tables["EP0002"]);

		}






	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		strcpy(s.sysmsg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return(doFlag);
}
