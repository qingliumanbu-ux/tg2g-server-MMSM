/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     李婧昊
Version:    1.0
Date:       2016-09-01
Description: 炼钢钢坯材料信息删除
**************************************************/
//框架头文件
#include "stdafx.h" 

/*<remark>=========================================================
/// <summary>
/// 炼钢钢坯材料信息删除
/// <para>
/// <para>
/// </summary>
/// <param name=""> </param>
/// <returns></returns>
===========================================================</remark>*/

//业务头文件
  


//外部函数声明
BM2_FUNCTION_IMPORT
int f_mmsm99(EIClass * bcls_rec,EIClass * bcls_ret,CDbConnection * conn);
BM2_FUNCTION_IMPORT
int f_wm00_queue(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_wmsm_e2t8m1_miss(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_t8z_23m_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//发送专家系统数据

BM2F_ENTERACE(mmsm01a1f5_del)                                            

int f_mmsm01a1f5_del(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");    

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tpssm03("TPSSM03");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);	

	EIClass in_23m;
	in_23m.Tables[0].Columns.Add(DT_STRING, "TC_NO");
	in_23m.Tables[0].Rows.Add();
	in_23m.Tables[0].Rows[0]["TC_NO"] = "T82322";
	in_23m.Tables.Add();
	in_23m.Tables[1].Columns.Add(tmmsm01);

	try
	{
		datetime	=	CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 添加与设置块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099"); 
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_LINE_TYPE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"SYSTEM_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"FUNC_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MAT_NO");
		}
		//入库队列生成 
		blkNum = bcls_rec->AtBlkName("WM00QUE");
		if (blkNum <= 0)
		{
			blkNum = bcls_rec->AddBlock();
			bcls_rec->SetBlkName(blkNum, "WM00QUE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER_DIV");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NUM");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "PLAN_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "PLAN_EXEC_SEQ_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "TRANS_TOOL");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "PRE_UNIT_CODE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "NEXT_UNIT_CODE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_DESTION");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "OPER_FLAG");
			bcls_rec->Tables["WM00QUE"].Rows.Add(); // 创建一行
		}			

		/* 获取输入参数 */
		for(int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm01["MAT_NO"]	= bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString().Trim();

			//Log::Trace("",__FUNCTION__,"tmmsm01.MAT_NO		= [{0}]",(const char*)tmmsm01["MAT_NO"].ToString());	

			/* 检查输入参数合法性 */
			if(tmmsm01["MAT_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,"材料号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		
			/* 查询材料主档 */			 
			tmmsm01.Query("MAT_NO");
			tmmsm01.TrimOrBlank();

			/* 校验逻辑数据 */			 
			if(tmmsm01["MAT_ID"].ToString().Trim() == "")
			{
				sprintf(s.msg,"材料[%s]不在当前档!",(const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			Log::Trace("",__FUNCTION__,"tmmsm01.MAT_ACT_WT = [{0}]",(const char*)tmmsm01["MAT_ACT_WT"].ToString());	
			if (tmmsm01["MAT_ACT_WT"].ToDecimal() != 0)
			{
				sprintf(s.msg, "材料[%s]系统重量不为0,不能删除!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			/*if(tmmsm01["HOLD_FLAG"].ToString().Trim() !=	"2")
			{
				sprintf(s.msg,"没有质量封锁[%s]的材料[%s]不能删除!",(const char*)tmmsm01["HOLD_FLAG"].ToString() ,(const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/
		/*	if (tmmsm01["IN_FLAG"].ToString().Trim() == "1")
			{
				sprintf(s.msg, "已入库的材料[%s]不能删除!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/
			/*if (tmmsm01["MAT_ORIGIN"].ToString().Trim() == "1")
			{
				sprintf(s.msg, "外购的材料[%s]不能删除!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/
			/*if(tmmsm01["REPAIR_FLAG"].ToString().Trim() == "1")
			{
				sprintf(s.msg,"材料[%s]目前正在返修中,不允许删除!",(const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if(tmmsm01["TRANSFER_FLAG"].ToString().Trim() == "1")
			{
				sprintf(s.msg,"材料[%s]已经编入转库计划中,不允许删除!",(const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if(tmmsm01["ORDER_NO"].ToString().Trim() != "")
			{
				sprintf(s.msg,"材料[%s]有合同信息,不允许删除!",(const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}*/
			if (tmmsm01["RCV_MAT_FLAG"].ToString().Trim() == "S" || tmmsm01["RCV_MAT_FLAG"].ToString().Trim() == "W")
			{
				sprintf(s.msg, "材料[%s]已经收货,不允许删除!", (const char*)tmmsm01["MAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}


			//带有虚拟板坯的坯子将虚拟板坯回退成未开始状态
			for (int i = 1; i < 13; i++)
			{
				if (tmmsm01["PONO_SLAB_" + CConvert::ToString(i)].ToString().Trim() != "")
				{
					tpssm03.Reset();
					tpssm03["SLAB_NO"] = tmmsm01["PONO_SLAB_" + CConvert::ToString(i)].ToString().Trim();
					tpssm03["SLAB_PROD_FLAG"] = "0";
					tpssm03.Update("SLAB_PROD_FLAG","SLAB_NO");
				}
			}

			/*调用仓库入库队列*/
			bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"]	= "1M"; //1M-盘盈入库
			bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NO"]			= tmmsm01["MAT_NO"];
			bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NUM"]			= tmmsm01["MAT_NUM"];
			bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_NO"]			= tmmsm01["STOCK_NO"];
			bcls_rec->Tables["WM00QUE"].Rows[0]["OPER_FLAG"]		= "D";
			//doFlag = f_wm00_queue(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}	

			EIClass miss_E2T8M1;
			miss_E2T8M1.Tables[0].set_TableName("E2T8M1");
			miss_E2T8M1.Tables["E2T8M1"].Columns.Add(DT_STRING, "MAT_NO");
			miss_E2T8M1.Tables["E2T8M1"].Rows.Add();
			miss_E2T8M1.Tables["E2T8M1"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"].ToString();

			doFlag = f_wmsm_e2t8m1_miss(&miss_E2T8M1, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/* 设置物料跟踪参数 */			 			
			bcls_rec->Tables["MM0099"].Rows.Add(); // 创建一行
			bcls_rec->Tables["MM0099"].Rows[i]["EVENT_ID"]			= "MM04"; 
			bcls_rec->Tables["MM0099"].Rows[i]["EVENT_LINE_TYPE"]	= "SM"; 
			bcls_rec->Tables["MM0099"].Rows[i]["SYSTEM_ID"]			= "MMSM"; 
			bcls_rec->Tables["MM0099"].Rows[i]["FUNC_ID"]			= "mmsm01a1f5_del"; 
			bcls_rec->Tables["MM0099"].Rows[i]["MAT_NO"]			= tmmsm01["MAT_NO"];

			tmmsm01.MergeTo(in_23m.Tables[1]);
			in_23m.Tables[1].Rows[0]["MAT_STATUS"] = "D";
		}	
					
		/* 调用物料函数 */			 			
		doFlag = f_mmsm99(bcls_rec,bcls_ret,conn);
		if(doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
#pragma region 调用函数，发送智慧质量电文
		if (in_23m.Tables[1].Rows.get_Count() > 0)
		{
			doFlag = f_t8z_23m_snd(&in_23m, bcls_ret, conn);
		}
#pragma endregion
	
 	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch(CApplicationException& ex)
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

