/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     王建征
Version:    1.0
Date:       2020-07-14
Description: 炼钢期初数据生成物料编码
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢期初数据验证
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
//#include "tmmsmbg.h"  
 


//外部函数声明
BM2_FUNCTION_EXPORT
//int f_mm0099_mat_code(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);//物料编码生成

BM2F_ENTERACE(mmsmbg_mat_code)

int f_mmsmbg_mat_code(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");
	CString	erro_flag("0");//数据校验标记
	CString	erro_info("");//数据校验信息

	/* 实体类定义 */ 	
	//CTMMSMBG tmmsmbg(conn);
	CModel tmmsm01qc("TMMSM01QC");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq_temp(conn);

	//期初数据DBLink名HBC1.DBLINK_HBC1

	/* 添加并设置块名 */
	blkNum = bcls_rec->Tables.IndexOf("MM0099");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("MM0099");
	}

	/*生成物料编码函数用*/
	blkNum = bcls_rec->Tables.IndexOf("MM0099_MAT_NO");
	if (blkNum < 0)
	{
		bcls_rec->Tables.Add("MM0099_MAT_NO");
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_STRING, "MAT_SHAPE_FLAG");//材料形态
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_STRING, "SG_SIGN");//牌号
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_STRING, "SG_STD");//标准
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_STRING, "PSC");//冶金规范码
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_STRING, "WHOLE_BACKLOG_CODE");//工序码
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_DECIMAL, "MAT_THICK");//厚度
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_DECIMAL, "MAT_WIDTH");//宽度
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_STRING, "DELIVY_STATUS");//交货状态
		bcls_rec->Tables["MM0099_MAT_NO"].Columns.Add(DT_STRING, "HOT_TREAT_METHOD_CODE");//热处理方式
	}
	bcls_rec->Tables["MM0099_MAT_NO"].Rows.Clear();

	try
	{
		//获取系统当前时刻
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//重置结构体待用
			tmmsm01qc.Reset();

			//获取输入参数
			tmmsm01qc.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			//物料编码
			bcls_rec->Tables["MM0099_MAT_NO"].Rows.Clear();
			bcls_rec->Tables["MM0099_MAT_NO"].Rows.Add();
			bcls_rec->Tables["MM0099_MAT_NO"].Rows[0]["MAT_SHAPE_FLAG"] = tmmsm01qc["MAT_SHAPE_FLAG"];
			bcls_rec->Tables["MM0099_MAT_NO"].Rows[0]["SG_SIGN"] = tmmsm01qc["SG_SIGN"];
			bcls_rec->Tables["MM0099_MAT_NO"].Rows[0]["SG_STD"] = tmmsm01qc["SG_STD"];
			bcls_rec->Tables["MM0099_MAT_NO"].Rows[0]["PSC"] = "0";
			bcls_rec->Tables["MM0099_MAT_NO"].Rows[0]["DELIVY_STATUS"] = "0";
			bcls_rec->Tables["MM0099_MAT_NO"].Rows[0]["HOT_TREAT_METHOD_CODE"] = "0";
			bcls_rec->Tables["MM0099_MAT_NO"].Rows[0]["WHOLE_BACKLOG_CODE"] = "A1";
			bcls_rec->Tables["MM0099_MAT_NO"].Rows[0]["MAT_THICK"] = tmmsm01qc["MAT_THICK"];
			bcls_rec->Tables["MM0099_MAT_NO"].Rows[0]["MAT_WIDTH"] = tmmsm01qc["MAT_WIDTH"];
			//doFlag = f_mm0099_mat_code(bcls_rec, bcls_ret, conn);
			//if (doFlag != 0)
			//{
			//	//throw CApplicationException(-1, s.msg, log.Location);
			//}
			tmmsm01qc["PRODUCT_CODE"] = bcls_rec->Tables["MM0099_MAT_NO"].Rows[0]["PRODUCT_CODE"].ToString();

			Log::Trace("", __FUNCTION__, "tmmsmbg.PRODUCT_CODE = [{0}]", tmmsm01qc["PRODUCT_CODE"].ToString());

			tmmsm01qc.MergeTo(bcls_ret->Tables[0], false);
		}
	   
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
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
	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;

}
		
