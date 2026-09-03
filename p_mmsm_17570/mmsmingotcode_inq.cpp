/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    3.0
Date:     2014-05-8
Description: 根据锭型代码获取相关信息
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/
//#include "SQLDDL.h"
#if defined _SYS_MMS || defined _SYS_MES     //MMS层或MES系统部署时调用的函数

#endif


int f_mmsm_unitwt(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

// service入口
BM2F_ENTERACE(mmsmingotcode_inq)

int f_mmsmingotcode_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量***** */
	int doFlag = 0;

	int blknum;
	int count = 0;
	CString v_factory_div = "";
	CString ingotcode = "";
	CString operatorType = ""; //小代码MS42
	CString totalNetWt = "";
	CString totalTube = "";
	CString wtPerMeter = "";
	CString slabType = "";
	CString ingotName = "";
	CString slabThick = "";
	CString slabWidth = "";
	CString slabLen = "";
	bool ifneed = 0;
	CString ifneedString = "";

	CString sqlstr = "";
	CString  sqlstr_inq = "";
	CString  sqlstr_where = "";

	CDbCommand cmd_inq(conn); //与DB 建立连接。
	CDbCommand cmd_sql(conn); //与DB 建立连接。

#if defined _SYS_MMS || defined _SYS_MES    //MMS层或MES系统部署时调用的函数
	CModel tqmtmd9("TQMTMD9");
#endif
	CModel tmmsmwt("TMMSMWT");

	try
	{
		tmmsmwt.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		//获得输入参数
		//============
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
			v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("INGOT_CODE"))
			ingotcode = bcls_rec->Tables[0].Rows[0]["INGOT_CODE"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("IF_NEED"))
			ifneedString = bcls_rec->Tables[0].Rows[0]["IF_NEED"].ToString().Trim();
		if (ifneedString == "True"){
			ifneed = 1;
		}

		//Log::Info("", __FUNCTION__, "v_factory_div  =[{0}] ingotcode  =[{1}] ifneedString  =[{2}] ifneed  =[{3}]", v_factory_div, ingotcode, ifneedString, ifneed);
		
#if defined _SYS_MMS || defined _SYS_MES    //MMS层或MES系统部署时调用的函数
		if (!ifneed && ingotcode != ""){
			sqlstr_inq = " SELECT * FROM TQMTMD9 ";

			sqlstr_where = " WHERE 1 = 1 AND  INGOT_CODE = @ingotcode";

			sqlstr = sqlstr_inq + sqlstr_where;
			//Log::Info("", __FUNCTION__, "sqlstr      =[{0}]", sqlstr);

			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.Parameters.Set("ingotcode", ingotcode);
			cmd_sql.ExecuteReader(); //执行读取

			//相关信息初始化。
			//===============
			if (cmd_sql.Read()) //只读取单记录，可用IF 语句。
			{
				cmd_sql.Fetch(tqmtmd9);		 //整个表结构的获取。
				//Log::Info("", __FUNCTION__, " tqmtmd9.BILLET_TYPE  =[{0}] tqmtmd9.INGOT_NAME  =[{1}] tqmtmd9.SLAB_THICK  =[{2}] tqmtmd9.SLAB_WIDTH  =[{3}] tqmtmd9.SLAB_LEN  =[{4}]", tqmtmd9["BILLET_TYPE"].ToString(), tqmtmd9["INGOT_NAME"].ToString(), tqmtmd9["SLAB_THICK"].ToDecimal(), tqmtmd9["SLAB_WIDTH"].ToDecimal(), tqmtmd9["SLAB_LEN"].ToDecimal());
				//bcls_ret->Tables[0].Rows.Add();
				bcls_rec->Tables[0].Rows[0]["SLAB_TYPE"] = tqmtmd9["BILLET_TYPE"];
				bcls_rec->Tables[0].Rows[0]["INGOT_NAME"] = tqmtmd9["INGOT_NAME"];
				bcls_rec->Tables[0].Rows[0]["SLAB_THICK"] = tqmtmd9["SLAB_THICK"];
				bcls_rec->Tables[0].Rows[0]["SLAB_WIDTH"] = tqmtmd9["SLAB_WIDTH"];
				bcls_rec->Tables[0].Rows[0]["SLAB_LEN"] = tqmtmd9["SLAB_LEN"];
				if (tqmtmd9["BILLET_TYPE"].ToString() == "5"){
					bcls_rec->Tables[0].Rows[0]["WT_PER_METER"] = tqmtmd9["INGOT_UNIT_WT"];
				}
			}
			cmd_sql.Close(); //关闭游标
		}
#endif

		doFlag = f_mmsm_unitwt(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
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

