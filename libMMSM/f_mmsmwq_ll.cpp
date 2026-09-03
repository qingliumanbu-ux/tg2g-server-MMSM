
/// <summary>
/// 功能说明:根据回炉信息，将实绩的重量及过钢量插入到表里，原炉号扣除，新炉号新增
/// </summary> 

#include "stdafx.h"


BM2_FUNCTION_EXPORT
int f_mmsmwq_ll(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int 	doFlag = 0;
	int 	ret = 0;
	int 	fetchRowCount = 0;
	CString heat_no = " ";
	CString ret_heat_no = " ";
	CString sqlstr = " ";
	CDecimal all_wt = 0;



	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_1(conn);

	CModel tmmsmwq_ll("TMMSMWQ_LL");


	try
	{
		tmmsmwq_ll.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsmwq_ll.Insert
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
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


