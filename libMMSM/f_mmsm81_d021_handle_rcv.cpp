/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   herui
Version:    1.0
Date:     2024-1-23
Description: 一钢计量实绩接收
**************************************************/

#include "stdafx.h"

int f_mmsm81_ins_h(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2_FUNCTION_EXPORT

int f_mmsm81_d021_handle_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	/*系统日志类定义*/
	CTracer log(__FUNCTION__);

	// 程序内部变量
	int doFlag = 0;
	int n_flag = 0;
	int	blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString cs_mmjl_seq_no = "";

	// 实体类定义
	CModel tmmsm81_rcv("TMMSM81_RCV");//计量单表电文履历

	// 数据库SQL操作字符串
	CString sqlstr("");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_ins(conn);
	CDbCommand cmd_upd(conn);
	CString auart = " ";
	EIClass eitable;

	try
	{
		// 获得当前时间
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		//计量系统传入参数接收
		/*CString ZCHO_JLBD.WEIGH_NO;   //磅单号
		CString ZCHO_JLBD.WEIGH_APP_NO;   //计量委托号
		CString ZCHO_JLBD.CAR_NO;   //车号
		CString ZCHO_JLBD.MAT_CODE;   //物料代码
		CString ZCHO_JLBD.MAT_CNAME;   //物料名称
		CDecimal ZCHO_JLBD.GROSS_WGT;   //毛重
		CDecimal ZCHO_JLBD.TARE_WGT;   //皮重
		CDecimal ZCHO_JLBD.NET_WGT;   //净重
		CString ZCHO_JLBD.GROSS_TIME;   //毛重时间
		CString ZCHO_JLBD.TARE_TIME;   //过皮时间
		CDecimal ZCHO_JLBD.DEDUCT_WGT;   //扣重
		CString ZCHO_JLBD.DEDUCT_DESC;   //扣重说明
		CDecimal ZCHO_JLBD.NET_WEIGHT1;   //二次净重
		CString ZCHO_JLBD.X_ITEM;   //行项目号
		CString ZCHO_JLBD.VOUCHER_CODE;   //凭证号
		CString ZCHO_JLBD.MAT_PILE_NO;   //批次号
		CString ZCHO_JLBD.WL_WORK_SEQ_NO;   //实绩号
		CString ZCHO_JLBD.SEND_UNIT;   //发货单位
		CString ZCHO_JLBD.SEND_UNIT_CODE;   //发货单位代码
		CString ZCHO_JLBD.RECV_UNIT;   //收货单位
		CString ZCHO_JLBD.RECV_UNIT_CODE;   //收货单位代码
		CString ZCHO_JLBD.LOAD_NAME;   //装点名称
		CString ZCHO_JLBD.LOAD_CODE;   //装点代码
		CString ZCHO_JLBD.UNLOAD_NAME;   //卸点名称
		CString ZCHO_JLBD.UNLOAD_CODE;   //卸点代码*/

		//tmmsm81_rcv["REC_CREATE_TIME"] = datetime;
		for (int i = 0; i < bcls_rec->Tables.get_Count(); i++)
		{
			Log::Trace("", "", "--表名：[{0}]---", bcls_rec->Tables[0].get_TableName());
		}
		tmmsm81_rcv["WEIGH_NO"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["WEIGH_NO"].ToString();//磅单号

		Log::Trace("", __FUNCTION__, "WEIGH_NO = [{0}]", bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["WEIGH_NO"].ToString());

		//tmmsm81_rcv["WEIGH_NO"] = "1234567890";
		tmmsm81_rcv["TRUST_ID"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["WEIGH_APP_NO"].ToString();//计量委托号
		tmmsm81_rcv["SHIP_NAME"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["CAR_NO"];//车号
		tmmsm81_rcv["MAT_CODE"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["MAT_CODE"];//物料代码
		tmmsm81_rcv["MAT_NAME"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["MAT_CNAME"];//物料名称

		Log::Trace("", __FUNCTION__, "TRUST_ID = [{0}]", bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["WEIGH_APP_NO"].ToString());
		Log::Trace("", __FUNCTION__, "SHIP_NAME = [{0}]", bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["CAR_NO"].ToString());
		Log::Trace("", __FUNCTION__, "MAT_CODE = [{0}]", bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["MAT_CODE"].ToString());
		Log::Trace("", __FUNCTION__, "MAT_NAME = [{0}]", bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["MAT_CNAME"].ToString());
		//重量单位Kg
		tmmsm81_rcv["MEASURE_UNIT"] = "KG";
		tmmsm81_rcv["GROSS_WT"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["GROSS_WGT"].ToDecimal();//毛重
		tmmsm81_rcv["TARE_WT"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["TARE_WGT"].ToDecimal();//皮重
		CDecimal net_wgt = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["NET_WGT"].ToDecimal();//净重
		tmmsm81_rcv["NET_WT"] = net_wgt;
		if (net_wgt > 0)
		{
			tmmsm81_rcv["STOCK_WT"] = tmmsm81_rcv["NET_WT"];
		}
		/*else
		{
			tmmsm81_rcv["STOCK_WT"] = tmmsm81_rcv["GROSS_WT"];
		}*/
		Log::Trace("", __FUNCTION__, "GROSS_WT = [{0}]", bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["GROSS_WGT"].ToString());
		Log::Trace("", __FUNCTION__, "TARE_WT = [{0}]", bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["TARE_WGT"].ToString());
		Log::Trace("", __FUNCTION__, "NET_WT = [{0}]", bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["NET_WGT"].ToString());

		tmmsm81_rcv["GROSS_TIME"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["GROSS_TIME"].ToString();//毛重时间
		tmmsm81_rcv["TARE_TIME"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["TARE_TIME"].ToString();//过皮时间
		tmmsm81_rcv["BUCKLE_WT"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["DEDUCT_WGT"].ToDecimal();//扣重
		tmmsm81_rcv["BUCKLE_REMARK"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["DEDUCT_DESC"].ToString();//扣重说明
		tmmsm81_rcv["SECOND_NET_WT"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["NET_WEIGHT1"].ToDecimal();//二次净重

		tmmsm81_rcv["PROJECT_NO"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["X_ITEM"].ToString();//行项目号
		tmmsm81_rcv["VOUCHER_ID"] = tmmsm81_rcv["VOUCHER_CODE"] = 
			bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["VOUCHER_CODE"].ToString();//凭证号
		CString strVoucherID = tmmsm81_rcv["VOUCHER_CODE"];

		sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT,CODE_DESC_2_CONTENT FROM TEP0002 t WHERE CODE_CLASS ='MMLC04' and CODE = @CODE   ";
		
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("CODE", strVoucherID.Substring(0, 2));
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			auart = cmd_inq.GetString(2);
			tmmsm81_rcv["AUART"] = cmd_inq.GetString(2);
			tmmsm81_rcv["BUSI_TYPE"] = cmd_inq.GetString(3);
		}
		else
		{
			cmd_inq.Close();
			sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT,CODE_DESC_2_CONTENT FROM TEP0002 t WHERE CODE_CLASS ='MMLC04' and CODE = @CODE   ";
			cmd_upd.SetCommandText(sqlstr);
			cmd_upd.Parameters.Set("CODE", strVoucherID.Substring(0, 4));
			cmd_upd.ExecuteReader();
			if (cmd_upd.Read())
			{
				auart = cmd_upd.GetString(2);
				tmmsm81_rcv["AUART"] = cmd_upd.GetString(2);
				tmmsm81_rcv["BUSI_TYPE"] = cmd_upd.GetString(3);
			}
			cmd_upd.Close();
		}
		cmd_inq.Close();
		if (auart.Trim()=="")
		{
			tmmsm81_rcv["AUART"] = "A";
			tmmsm81_rcv["BUSI_TYPE"] = "调拨";
		}

		////根据凭证号判断业务类型
		//if (strVoucherID.Substring(0, 4) == "21PS" /*|| strVoucherID.Substring(0, 4) == "21JF"*/||
		//	strVoucherID.Substring(0, 2) == "C0") //配送
		//{
		//	tmmsm81_rcv["AUART"] = "C";
		//	tmmsm81_rcv["BUSI_TYPE"] = "配送";
		//}
		//else
		//{
		//	if (strVoucherID.Substring(0, 2) == "PO") /*|| strVoucherID.Substring(0, 2) == "C0" *//*|| strVoucherID.Substring(0, 2) == "AB"*/
		//	/*	|| strVoucherID.Substring(0, 2) == "AT"*/ //直供
		//	{
		//		tmmsm81_rcv["AUART"] = "B";
		//		tmmsm81_rcv["BUSI_TYPE"] = "直供";
		//	}
		//	else //调拨
		//	{
		//		tmmsm81_rcv["AUART"] = "A";
		//		tmmsm81_rcv["BUSI_TYPE"] = "调拨";
		//	}
		//}

		if (strVoucherID.Substring(0, 4) == "21JF")
		{
			tmmsm81_rcv["AUART"] = "D";
			tmmsm81_rcv["BUSI_TYPE"] = "交废";
		}
		if (strVoucherID.SubstringNE(0, 4) == "21XH")
		{
			tmmsm81_rcv["AUART"] = "E";
			tmmsm81_rcv["BUSI_TYPE"] = "自循环废钢";
		}
		
		Log::Trace("", __FUNCTION__, "凭证号 = [{0}]", bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["VOUCHER_CODE"].ToString());

		tmmsm81_rcv["LOT_NO"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["MAT_PILE_NO"].ToString();//批次号
		tmmsm81_rcv["MISSING_NO"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["WL_WORK_SEQ_NO"].ToString();//实绩号
		Log::Trace("", __FUNCTION__, "实绩号 = [{0}]", bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["WL_WORK_SEQ_NO"].ToString());
		//tmmsm81_rcv["SRC_STOCK_CODE"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["SEND_UNIT"].ToString();//发货单位
		tmmsm81_rcv["SRC_STOCK_CODE"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["SEND_UNIT_CODE"].ToString();//发货单位代码
		tmmsm81_rcv["RECV_DEPT_NAME"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["RECV_UNIT"].ToString();//收货单位
		tmmsm81_rcv["RECV_DEPT_CODE"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["RECV_UNIT_CODE"].ToString();//收货单位代码
		//tmmsm81_rcv["PROJECT_NO"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["LOAD_NAME"].ToString();//装点名称
		tmmsm81_rcv["LADE_CODE"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["LOAD_CODE"].ToString();//装点代码
		//tmmsm81_rcv["PROJECT_NO"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["UNLOAD_NAME"].ToString();//卸点名称
		tmmsm81_rcv["UNLOAD_POINT_CODE"] = bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["UNLOAD_CODE"].ToString();//卸点代码
		Log::Trace("", __FUNCTION__, "卸点代码 = [{0}]", bcls_rec->Tables["ZCHO_JLBD_RFC"].Rows[0]["UNLOAD_CODE"].ToString());
		tmmsm81_rcv.TrimOrBlank();

		tmmsm81_rcv["TRNP_MODE_CODE"] = "3";//运输方式

		/* 检查输入参数合法性 */
		if (tmmsm81_rcv["WEIGH_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg, "[计量单号]不能为空！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm81_rcv["MAT_CODE"].ToString().Trim() == "")
		{
			strcpy(s.msg, "[物料代码]不能为空！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm81_rcv["GROSS_WT"].ToString().Trim() == "")
		{
			strcpy(s.msg, "[毛重]不能为空！");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm81_rcv["RECV_DEPT_CODE"].ToString().Trim() != "")
		{
			if (tmmsm81_rcv["RECV_DEPT_CODE"].ToString().SubstringNE(0,4) == "6220" )
			{
				strcpy(s.msg, "计量信息不是北区数据！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (strVoucherID.Substring(0, 2) == "AT" || strVoucherID.Substring(0, 4) == "22JF" || strVoucherID.Substring(0, 4) == "22PS")
		{
			strcpy(s.msg, "计量信息不是北区数据！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsm81_rcv["REC_CREATE_TIME"] = tmmsm81_rcv["RECEIVE_DATA_TIME"] = datetime;
		tmmsm81_rcv["FORM_EDIT_FLAG"] = '0';
		tmmsm81_rcv.Insert();

		n_flag = f_mmsm81_ins_h(&eitable, bcls_ret, conn);
		if (n_flag < 0 )
		{
			sprintf(s.msg, "记录表操作失败!");
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);

		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (const CApplicationException& ex)
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
