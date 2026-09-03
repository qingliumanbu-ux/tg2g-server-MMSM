/***********************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   mfj
Version:    1.0
Date:     2024-01-08
Description: 废钢实绩增删改
*************************************************************/
/***** C/C++ 的标准头文件部分 *****/
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件



int f_mm0011(CString SeqName, CDecimal SeqLen, CString &SeqNo, CDbConnection * conn);	//获取流水号
int f_mmsm_210044_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

//外部函数声明
BM2_FUNCTION_EXPORT
int f_mmsm39_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{

	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm39_proc";                //定义函数英文名称  
	CString FunctionCname = "切废信息增删改";          //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义


	//程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   blkNum;

	CString sqlstr = "";
	CString v_proc_div = "";
	CString v_pract_rcv_flag = "";
	CString v_factory_div = "";
	CString v_station_id = "";
	CString v_acjc_relation_id = "";
	CString PROD_SHIFT_NO = "";
	CString PROD_SHIFT_GROUP = "";
	CString v_resume_seq_no = "";//序号
	CString v_operate = "";//操作区分	 I 新增    U 修改
	CString v_sap_erp_matnr = "";//物料编码
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	
	EIClass inBlock1;
	EIClass outBlock1;


	try
	{
		CPageInfo pageInfo;

		/*数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

	
		/* 实体类定义 */
		CModel tmmsm39("TMMSM39");
		CModel tmmsm01("TMMSM01"); 
		CModel tmmsm96("TMMSM96");
		CModel tmmsm39_1("TMMSM39_1");
		

		//初始化实体类
		tmmsm39.Reset();


		EIClass bcls_rec_210044;//发送L4二切实绩电文
		bcls_rec_210044.Tables[0].set_TableName("210044");
		bcls_rec_210044.Tables[0].Columns.Add(tmmsm39);
		bcls_rec_210044.Tables[0].Columns.Add(DT_STRING, "DEAL_FLAG");
		


		if (bcls_rec->Tables.Contains("TMMSM39"))
		{
			for (int i = 0; i < bcls_rec->Tables["TMMSM39"].Rows.get_Count(); i++)
			{
				tmmsm39.Reset();//将上一循环数据清空
				tmmsm39.MergeFrom(bcls_rec->Tables["TMMSM39"].Rows[i]);
				//判废
				if ("QM05" == bcls_rec->Tables["TMMSM39"].Rows[i]["EVENT_ID"].ToString().Trim())
				{
					doFlag = f_mm0011("TMMSM39_seq", 8, v_resume_seq_no, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}

					tmmsm39["CUT_AFTER_WIDTH"] = tmmsm39["MAT_WIDTH"];
					tmmsm39["CUT_AFTER_LEN"] = tmmsm39["MAT_LEN"];
					tmmsm39["CUT_AFTER_THICK"] = tmmsm39["MAT_THICK"];

					//画面切废，规格取切后长宽厚
					tmmsm39["CUT_AFTER_WIDTH"] = tmmsm39["MAT_ACT_WIDTH"];
					tmmsm39["CUT_AFTER_LEN"] = tmmsm39["MAT_ACT_LEN"];
					tmmsm39["CUT_AFTER_THICK"] = tmmsm39["MAT_ACT_THICK"];
					//tmmsm39["CUT_AFTER_WT"] = tmmsm39["MAT_WT"];//切后重量
					//tmmsm39["CUT_AFTER_WT"] = 0;  //待定
					tmmsm39["CUT_SCRAP_WT"] = tmmsm39["MAT_ACT_WT"];//切废量等于实际重量 --- 此为判废
					//tmmsm39["CUT_AFTER_WT"] = tmmsm39["REAL_TIME_WT"];//实时重量
					//tmmsm39["CUT_AFTER_WT"] = tmmsm39["QUALIFIED_WT"];//合格产量
					tmmsm39["FINISH_FLAG"] = "9";//1 待反馈   3 删除待反馈 9处理成功   头尾坯且切头切尾类型的走物料同步，不走反馈
					tmmsm39["RESUME_SEQ_NO"] = datetime + v_resume_seq_no;
					tmmsm39["SAP_ERP_MATNR"] = "判废";
					tmmsm39["RECUT_DATE"] = datetime;
					tmmsm39.TrimOrBlank();
					tmmsm39.Insert();

					tmmsm39_1.CopyFrom(tmmsm39);//记录履历
					tmmsm39_1.TrimOrBlank();
					tmmsm39_1.Insert();

					bcls_rec_210044.Tables[0].Rows.Add();
					bcls_rec_210044.Tables[0].Rows[i].Merge(tmmsm39);
					bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "N";//新增
				}
				//判废取消
				else if ("QM06" == bcls_rec->Tables["TMMSM39"].Rows[i]["EVENT_ID"].ToString().Trim())
				{
					tmmsm39["SAP_ERP_MATNR"] = "判废";
					tmmsm39.TrimOrBlank();
					tmmsm39.Delete("SAP_ERP_MATNR,MAT_NO");

					bcls_rec_210044.Tables[0].Rows.Add();
					bcls_rec_210044.Tables[0].Rows[i].Merge(tmmsm39);
					bcls_rec_210044.Tables[0].Rows[0]["DEAL_FLAG"] = "D";//删除
				}
			}

			//判废有单独电文，此处不发电文  只存实绩表
			if (false)
			{
				if (bcls_rec_210044.Tables[0].Rows.get_Count()>0)
				{
					/********   太钢定制 发送L4电文 钢坯切废实绩   ***********/
					doFlag = f_mmsm_210044_snd(&bcls_rec_210044, bcls_ret, conn);
					if (doFlag < 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			
			
		}
		

















	}
	/*捕获数据库操作异常*/
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		//LogTrace(1,1,"%s",(const char*)sqlstr);
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = "DB error:" + sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应

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

	//LogTrace(1,1,"doFlag[%d]s.msg[%s],s.sysmsg[%s]",doFlag,s.msg,s.sysmsg);
	////LogTrace(1, 1, " **************%s end*****************", (const char*)FunctionEname);
	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;

}

