/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:    向萍
Version:    1.0
Date:       2015-08-24
Description: 炼钢材料分切电文接收
**************************************************/
//框架头文件
#include "stdafx.h" 
#include "epex.h" 


//业务头文件
  
 

//外部函数声明
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) ;

// service入口
BM2F_ENTERACE_TELE(cm_200035_rcv)

int f_cm_200035_rcv(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag	= 0;
	int blkNum	= 0;

	/* 业务变量 */
	CString	datetime("");    

	/* 实体类定义 */
	CModel tmmsm35("TMMSM35");
	CModel tmmsm35_main("TMMSM35");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm01_main("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	CDecimal matTheoryWt = 0;
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

  try
  { 
		datetime	=	CDateTime::Now().ToString("yyyyMMddHHmmss");

			
		if (bcls_rec->Tables.Contains("MM0099") == false)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_LINE_TYPE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"SYSTEM_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"FUNC_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MAT_NO");
		}
		
		EIClass bcls_rec_QM02;//材料表面判定
		bcls_rec_QM02.Tables[0].set_TableName("MM0099");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_CODE");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_MAKER");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SURFACE_DECIDE_TIME");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "DEFECT_CODE");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "DEFECT_CLASS");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SLAB_PLACE_CODE");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "SPARE_ITEM_0");
		bcls_rec_QM02.Tables[0].Columns.Add(DT_STRING, "MACH_CLEAR_FLAG");

		EIClass bcls_rec_QM18;//材料质量释放
		bcls_rec_QM18.Tables[0].set_TableName("MM0099");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "EVENT_ID");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "SYSTEM_ID");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "FUNC_ID");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "REL_REMARK");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "REL_MAKER");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "REL_TIME");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "DEFECT_CODE");
		bcls_rec_QM18.Tables[0].Columns.Add(DT_STRING, "DEFECT_CLASS");
			

		/* 获取输入参数 */
		tmmsm35.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		tmmsm01["MAT_NO"] = tmmsm35["IN_MAT_NO"];
		tmmsm01.Query();
		matTheoryWt = tmmsm01["MAT_THEORY_WT"];

		//Log::Trace("",__FUNCTION__,"入口材料 matTheoryWt理重	= [{0}]",matTheoryWt);



		if (tmmsm01["ORDER_NO"].ToString().Trim() != "")
		{
			CFormattable arguments[] = { tmmsm01["MAT_NO"].ToString() };
			CMessageFormat::Format(s.msg, "材料{0}为合同材,不允许分段处理", arguments, 1);
			throw CApplicationException(-1, s.msg, log.Location);
		}

	
		/* 获取被并批材料号 */
		for(int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++ ) 
		{
			/* 获取被并批材料 */
			tmmsm35["MAT_NO"]		= bcls_rec->Tables[1].Rows[i]["MAT_NO"].ToString().Trim();	//分切材料号

			/* 被分切材料号为空,则跳出循环 */	
			if(tmmsm35["MAT_NO"].ToString().Trim() == "")
			{
				break;
			}

			//Log::Trace("",__FUNCTION__,"分切材料号 tmmsm35.MAT_NO*******		= [{0}]",tmmsm35["MAT_NO"].ToString());

			tmmsm35["MAT_THICK"] = bcls_rec->Tables[1].Rows[i]["MAT_THICK"].ToDecimal();
			tmmsm35["MAT_WIDTH"] = bcls_rec->Tables[1].Rows[i]["MAT_WIDTH"].ToDecimal();
			tmmsm35["MAT_LEN"]   = bcls_rec->Tables[1].Rows[i]["MAT_LEN"].ToDecimal();
			tmmsm35["MAT_WT"]    = bcls_rec->Tables[1].Rows[i]["MAT_WT"].ToDecimal();
			tmmsm35["MAT_TUBE"]  = bcls_rec->Tables[1].Rows[i]["MAT_TUBE"].ToDecimal();
			tmmsm35.TrimOrBlank();
			tmmsm35.Insert();

		/*	
			//Log::Trace("",__FUNCTION__,"入口材料tmmsm35.IN_MAT_TUBE	= [{0}]",tmmsm35["IN_MAT_TUBE"].ToDecimal());
			//Log::Trace("",__FUNCTION__,"入口材料tmmsm35.IN_MAT_LEN	= [{0}]",tmmsm35["IN_MAT_LEN"].ToDecimal());
			//Log::Trace("",__FUNCTION__,"材料tmmsm35.MAT_TUBE	= [{0}]",tmmsm35["MAT_TUBE"].ToDecimal());
			//Log::Trace("",__FUNCTION__,"材料tmmsm35.MAT_LEN	= [{0}]",tmmsm35["MAT_LEN"].ToDecimal());*/

	
			tmmsm01["MAT_NO"] = tmmsm35["MAT_NO"];
			tmmsm01["IN_MAT_NO"] = tmmsm35["IN_MAT_NO"];
			tmmsm01["MAT_ACT_LEN"] = tmmsm35["MAT_LEN"];
			tmmsm01["MAT_LEN"] = tmmsm35["MAT_LEN"];
			tmmsm01["MAT_NUM"] = tmmsm35["MAT_TUBE"];
			tmmsm01["MAT_ACT_WT"] = tmmsm35["MAT_WT"];
			tmmsm01["MAT_THEORY_WT"] = matTheoryWt / tmmsm35["IN_MAT_TUBE"].ToDecimal() / tmmsm35["IN_MAT_LEN"].ToDecimal() * tmmsm35["MAT_TUBE"].ToDecimal() * tmmsm35["MAT_LEN"];
			tmmsm01["MAT_THEORY_WT"] = tmmsm01["MAT_THEORY_WT"].ToDecimal().Round(3);
			tmmsm01.Insert();

		
			bcls_rec->Tables["MM0099"].Rows.Add();
			bcls_rec->Tables["MM0099"].Rows[i]["EVENT_ID"] = "MM15";
			bcls_rec->Tables["MM0099"].Rows[i]["EVENT_LINE_TYPE"] = "SM";
			bcls_rec->Tables["MM0099"].Rows[i]["SYSTEM_ID"] = "MMSM";
			bcls_rec->Tables["MM0099"].Rows[i]["FUNC_ID"] = "cm_200035_rcv";
			bcls_rec->Tables["MM0099"].Rows[i]["MAT_NO"] = tmmsm01["MAT_NO"];

			bcls_rec_QM02.Tables[0].Rows.Add();
			bcls_rec_QM02.Tables[0].Rows[i]["EVENT_ID"] = "QM02";
			bcls_rec_QM02.Tables[0].Rows[i]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_QM02.Tables[0].Rows[i]["SYSTEM_ID"] = "MMSM";
			bcls_rec_QM02.Tables[0].Rows[i]["FUNC_ID"] = "mmsm35_cut";
			bcls_rec_QM02.Tables[0].Rows[i]["MAT_NO"] = tmmsm35["MAT_NO"];
			bcls_rec_QM02.Tables[0].Rows[i]["SURFACE_DECIDE_CODE"] = "1";// 1:合格
			bcls_rec_QM02.Tables[0].Rows[i]["SURFACE_DECIDE_MAKER"] = s.userid;
			bcls_rec_QM02.Tables[0].Rows[i]["SURFACE_DECIDE_TIME"] = dateNow;
			bcls_rec_QM02.Tables[0].Rows[i]["DEFECT_CODE"] = " ";
			bcls_rec_QM02.Tables[0].Rows[i]["DEFECT_CLASS"] = " ";
			bcls_rec_QM02.Tables[0].Rows[i]["SLAB_PLACE_CODE"] = tmmsm01["SLAB_PLACE_CODE"];
			bcls_rec_QM02.Tables[0].Rows[i]["SPARE_ITEM_0"] = "炼钢分切，自动表判合格。";
			bcls_rec_QM02.Tables[0].Rows[i]["MACH_CLEAR_FLAG"] = "";

			bcls_rec_QM18.Tables[0].Rows.Add();
			bcls_rec_QM18.Tables[0].Rows[i]["EVENT_ID"] = "QM18";
			bcls_rec_QM18.Tables[0].Rows[i]["EVENT_LINE_TYPE"] = "00";
			bcls_rec_QM18.Tables[0].Rows[i]["SYSTEM_ID"] = "MMSM";
			bcls_rec_QM18.Tables[0].Rows[i]["FUNC_ID"] = "mmsm35_cut";
			bcls_rec_QM18.Tables[0].Rows[i]["MAT_NO"] = tmmsm35["MAT_NO"];
			bcls_rec_QM18.Tables[0].Rows[i]["REL_REMARK"] = "分切释放";
			bcls_rec_QM18.Tables[0].Rows[i]["REL_MAKER"] = s.userid;
			bcls_rec_QM18.Tables[0].Rows[i]["REL_TIME"] = dateNow;
			bcls_rec_QM18.Tables[0].Rows[i]["DEFECT_CODE"] = " ";
			bcls_rec_QM18.Tables[0].Rows[i]["DEFECT_CLASS"] = " ";
					
				
		}


		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
		throw CApplicationException(-1, s.msg, log.Location);
		}


		bcls_rec->Tables["MM0099"].Rows.Clear();
		bcls_rec->Tables["MM0099"].Rows.Add();  

		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM16";
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "cm_200035_rcv";
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm35["IN_MAT_NO"];
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
		throw CApplicationException(-1, s.msg, log.Location);
		}

		Log::Trace("", __FUNCTION__, "------------事件QM02[表面判定]--------------");
		doFlag = f_mmsm99(&bcls_rec_QM02, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		Log::Trace("", __FUNCTION__, "------------事件QM18[质量释放]--------------");
		doFlag = f_mmsm99(&bcls_rec_QM18, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
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
