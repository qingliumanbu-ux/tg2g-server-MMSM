/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 吴振楠
日期: 2012-2-17
功能: 查询EPEP01
修改历史：
 日期:________；修改人：________; 需求提出人________
 变更内容:
**************************************************/  
#include "stdafx.h"
#include "tmmsmis1f.h"  //业务头
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient;

/*<remark>=========================================================
/// <summary>
/// 物料信息查询
/// <para>如果查询在线数据，根据查询条件查询冷轧物料主表TMMCR01</para>
/// <para>如果查询历史数据，根据查询条件查询冷轧物料历史表HMMCR01</para>
/// </summary>
/// <param name="CRRNT_HSTY_FLAG">在线档OR历史档标记</param>
/// <param name="MAT_NO">材料号</param>
/// <param name="FACTORY_DIV">厂别区分</param>
/// <param name="NEXT_UNIT_CODE">下道机组代码</param>
/// <param name="...">其他...</param>
/// <returns>满足查询条件的冷轧物料主表TMMCR01数据</returns>
===========================================================</remark>*/
BM2F_ENTERACE(mmsmis02a1_code)

int f_mmsmis02a1_code(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);//系统日志类定义
	
	/* 程序内部变量 */
	int doFlag = 0;
	int sql_flag = 0;
	int i = 0;

	CString sqlstr = " ";
	int	blkNum1	= 0;
	int	blkNum	= 0;
	/* 实体类定义 */
	CTMMSMIS1F tmmsmis1f(conn);
	try
	{
		CDbCommand cmdcount(conn);
		CDbCommand cmdinq(conn);
		
		CString strSql = "";
		CString strSql2 = "";
		CString v_code_class = bcls_rec->Tables[0].Rows[0]["code_class"];
	
		/*查询炉次命令信息 传入块1*/
		bcls_ret->Tables[0].set_TableName("EP0002"); 
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"CODE");
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"CODE_DESC_1_CONTENT");
		
		/*数据校验*/
		if(v_code_class.Trim().Compare(" ")<=0)
		{
			throw CApplicationException(_RES("MMSMS0000228")/*cast_lot_no不能为空*/);
			Log::Trace("",__FUNCTION__,"v_code_class 【{0}】不能为空",(const char*)v_code_class);
		}
		
		if(v_code_class.Trim() == "0")
		{
			/*查询SIPM.TSIPMOF02*/
			/*strSql = "SELECT WHOLE_BACKLOG_CODE as 全程工序代码,WHOLE_BACKLOG_NAME as 全程工序名称,UNIT_CODE as 机组代码,BACKLOG_POS as 工序位,"
				     "BACKLOG_VAL as 工序值 FROM SIPM.TSIPMOF02 ORDER BY BACKLOG_POS ASC";*/
			strSql = "SELECT WHOLE_BACKLOG_CODE,WHOLE_BACKLOG_NAME ,UNIT_CODE ,BACKLOG_POS ,"
				     "BACKLOG_VAL  FROM SIPM.TSIPMOF02 ORDER BY BACKLOG_POS ASC";
			cmdinq.SetCommandText(strSql);
			cmdinq.Prepare();
			Log::Trace("",__FUNCTION__,"strSql = 【{0}】",(const char*)strSql);
			cmdinq.ExecuteQuery(bcls_ret->Tables["EP0002"]);
			bcls_ret->Tables["EP0002"].Rows.Add();

		}
		else if(v_code_class.Trim() == "1")
		{
			/*查询SIPM.TSIPMOF01*/
			strSql = "SELECT WHOLE_BACKLOG_CODE ,WHOLE_BACKLOG_NAME ,UNIT_CODE ,BACKLOG_POS ,"
				     "BACKLOG_VAL  FROM SIPM.TSIPMOF01 ORDER BY BACKLOG_POS ASC";

			cmdinq.SetCommandText(strSql);
			cmdinq.Prepare();
			Log::Trace("",__FUNCTION__,"strSql = 【{0}】",(const char*)strSql);
			cmdinq.ExecuteQuery(bcls_ret->Tables["EP0002"]);
			bcls_ret->Tables["EP0002"].Rows.Add();
		}
	
		else
		{
			/*根据标记查询在线档或历史档  分页*/

			strSql = " SELECT DISTINCT CODE,CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = @code_class ORDER BY CODE asc";


			cmdinq.SetCommandText(strSql);
			cmdinq.Prepare();
			cmdinq.Parameters.Set("code_class",	v_code_class);
			Log::Trace("MMSM",__FUNCTION__,"strSql = 【{0}】",(const char*)strSql);
			cmdinq.ExecuteQuery(bcls_ret->Tables["EP0002"]);

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
		strcpy(s.sysmsg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return(doFlag);
}
