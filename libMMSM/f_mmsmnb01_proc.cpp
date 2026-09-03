/*************************************************
Copyright:	Baosight Software LTD.co Copyright (c) 2010
Author:		renximing
Version:    1.0
Date:		2023-12-15
Description:南北区互调
**************************************************/

#include "stdafx.h"

//程序用头文件
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_mmsmnb01_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString tcNO = " ";
	CString dealFlag = " ";
	CString tableName = " ";
	CString primaryKey = " ";
	CString primaryData = " ";
	EPEX epex;

	/* 实体类定义 */

	// 数据库SQL操作字符串
	CString  sqlstr("");
	CDecimal cd_seq_no = 0;
	CString	datetime("");
	datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CModel tmmsmnb01("TMMSMNB01");

	try
	{
		tmmsmnb01["TICODE"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["c_deliveryid"].ToString();					// 调拨单号
		tmmsmnb01["MAT_CODE"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["c_productid"].ToString();				// 物料代码
		tmmsmnb01["MAT_NAME"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["c_productname"].ToString();				// 物料名称
		tmmsmnb01["SOURCE_WERKS"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["c_senddept"].ToString();				// 发送工厂
		tmmsmnb01["FACTORY_CODE"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["c_acceptdept"].ToString();			// 接受工厂
		tmmsmnb01["SRC_LOC_CODE"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["c_sendstock"].ToString();			// 发送库房
		tmmsmnb01["DST_LOC_CODE"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["c_acceptstock"].ToString();			// 接受库房
		tmmsmnb01["IN_MAT_WT"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["n_sendamount"].ToString();				// 发送重量
		tmmsmnb01["MAT_WT"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["n_acceptamount"].ToString();					// 接收重量
		tmmsmnb01["C_SENDUSERID"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["c_senduserid"].ToString();			// 送料人
		tmmsmnb01["C_ACCEPTUSERID"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["c_acceptuserid"].ToString();		// 收料人
		tmmsmnb01["C_TRUCKNUM"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["c_trucknum"].ToString();				// 车号
		//tmmsmnb01["C_SENDCOSTCENTER"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["c_sendcostcenter"].ToString();	// 发料工厂成本中心
		//tmmsmnb01["R_PLANTCOSTCENTER"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["r_plantcostcenter"].ToString(); // 接收工厂成本中心
		tmmsmnb01["C_STATESIGN"] = bcls_rec->Tables["mes_mm_gm"].Rows[0]["c_statesign"].ToString();				// 调拨状态（1-未确认，2-接收，3-驳回）
		tmmsmnb01["STATUS1"] = "1";					// 数据来源 1电文 2 手动新增
		tmmsmnb01["REC_CREATOR"] = "T8T701";			//记录创建责任者
		tmmsmnb01["REC_CREATE_TIME"] = datetime;		//记录创建时刻
		/*tmmsm65["STATUS"] = "0";
		tmmsm65["USE_LOGO"] = "0";*/
		tmmsmnb01.TrimOrBlank();

		sqlstr = "  SELECT NVL(max(SEQ_NO),0) FROM  TMMSMNB01     WHERE 1=1   AND MAT_CODE= @MAT_CODE  ";
		cmd_inq.Parameters.Set("MAT_CODE", tmmsmnb01["MAT_CODE"].ToString());
		Log::Trace(" ", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
		Log::Trace(" ", __FUNCTION__, "sqlstr =[{0}]", tmmsmnb01["MAT_CODE"].ToString());
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cd_seq_no = cmd_inq.GetDecimal(1) + 1;
		}
		cmd_inq.Close();
		tmmsmnb01["SEQ_NO"] = cd_seq_no;

		if (tmmsmnb01.QueryCount("TICODE")>0)
		{
			tmmsmnb01.Delete("TICODE");
		}

		tmmsmnb01.Insert();

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
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

	return doFlag;

}
