/***********************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 吹氩实绩增删改
*************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

//外部函数声明
int f_mmsm53_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_sj_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);
int f_mmsm10_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

#if  defined _SYS_MES   || defined _SYS_PES
int f_pssm12_mm_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

#if  defined _SYS_PES
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

BM2_FUNCTION_EXPORT
 int f_mmsm22_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	/****** 定义函数名称 ***** */
CString FunctionEname = "f_mmsm22_proc";                //定义函数英文名称  
CString FunctionCname = "转炉信息增删改";          //定义函数中文名称


CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
  

  //程序用变量
  int   doFlag = 0;
  int   fetchRowCount = 0;
  int   i = 0;
  int   blkNum;

  CString sqlstr="";
  CString v_proc_div= "";
  CString v_pract_rcv_flag= "";
  CString v_factory_div = "";
  CString v_station_id = "";
  CDecimal v_ladle_down_temp = 0;

  CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
  CString PROD_SHIFT_NO = "";
  CString PROD_SHIFT_GROUP = "";
     
  try
  {
		CPageInfo pageInfo;	

		/*数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。

		/* 实体类定义 */
		CModel tmmsm22("TMMSM22");
		CModel tmmsm22_old("TMMSM22");
		CModel tmmsm2a("TMMSM2A");
		CModel tmmsm2b("TMMSM2B");
		CModel tmmsm00("TMMSM00");
		CModel ttmsm01("TTMSM01");
		CModel htmsm01("HTMSM01");
		CModel tpssm11("TPSSM11");
		CModel tmmsm21("TMMSM21");
        //初始化实体类
		tmmsm22.Reset();
		tmmsm2a.Reset();
		tmmsm2b.Reset();
		tmmsm00.Reset();
		
		//调用校验及公共处理函数,针对有共性的字段进行赋值

		if(!bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING,"TABLE_TYPE"); 
		}
	
		bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"] = "TMMSM22";

		doFlag = f_mmsm_sj_proc(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		tmmsm00.MergeFrom(bcls_ret->Tables["TMMSM00"].Rows[0]);
		tmmsm00.TrimOrBlank();

		/*如果是电文调用需要在电文接收service里对厂别和设备类型进行赋值*/
		if (bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))  //处理区分  I-新增  U-修改  D-删除
			v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))  //厂别
			v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("STATION_ID"))  //设备类型
			v_station_id = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString().TrimOrBlank().ToUpper();

		Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
		Log::Trace("", __FUNCTION__, "v_factory_div=[{0}]", v_factory_div);
		Log::Trace("", __FUNCTION__, "v_station_id=[{0}]", v_station_id);
		
		tmmsm22.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm22.TrimOrBlank();
		tmmsm22.CopyFrom(tmmsm00);
		
		if (tmmsm22["PROD_SHIFT_NO"].ToString().Trim() == "" )
		{
			f_epep_get_shift_group("SMDD", tmmsm22["END_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
			tmmsm22["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
		}
		//校验钢包
		ttmsm01["LADLE_NO"] = tmmsm22["LADLE_NO"];
		if (!ttmsm01.Query("LADLE_NO"))
		{
			strcpy(s.msg, "钢包号：" + ttmsm01["LADLE_NO"].ToString() + " 信息查询失败。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//生产日期暂时定为取开始时刻。  mfj  20231120
		if (tmmsm22["START_TIME"].ToString().Trim() != "" && tmmsm22["PROD_DATE"].ToString().Trim() == "")
		{
			tmmsm22["PROD_DATE"] = tmmsm22["START_TIME"].ToString().SubstringNE(0, 8);
		}

		if (tmmsm22["PROD_DATE"].ToString().Trim() != "")
		{
			tmmsm22["PROD_DATE"] = tmmsm22["PROD_DATE"].ToString().Substring(0, 8);
		}


		if(v_proc_div == "I")
		{
			//产品化三明模式，避免弹窗里保存按钮点击多次
			tmmsm22_old["HEAT_NO"] = tmmsm22["HEAT_NO"].ToString();
			tmmsm22_old["PROC_NO"] = tmmsm22["PROC_NO"].ToString();
			if (tmmsm22_old.QueryCount("HEAT_NO,PROC_NO") > 0)
			{
				strcpy(s.msg, "主实绩已保存!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm22["PROC_NO"] = tmmsm22["HEAT_NO"];
			if ("" == tmmsm22["START_TIME"].ToString().Trim() || "" == tmmsm22["END_TIME"].ToString().Trim())
			{
				strcpy(s.msg, "熔炼号：" + tmmsm22["LADLE_NO"].ToString() + " 进站时刻、出站时刻不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//计算处理时间
			tmmsm22["PROC_TIME"] = 0;
			if ("" != tmmsm22["START_TIME"].ToString().Trim() && "" != tmmsm22["END_TIME"].ToString().Trim())
			{
				CDateTime startDateTime = CDateTime::Parse(tmmsm22["START_TIME"].ToString());
				CDateTime endDateTime = CDateTime::Parse(tmmsm22["END_TIME"].ToString());
				CTimeSpan timeSpan = endDateTime - startDateTime;
				CDecimal diffTime = timeSpan.TotalMinutes();
				if (0 >= diffTime){
					strcpy(s.msg, "吹氩处理时间需大于0，请输入正确的进站时刻、出站时刻。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm22["PROC_TIME"] = diffTime;
			}
			Log::Trace("", __FUNCTION__, "tmmsm22[PROC_TIME]=[{0}]", tmmsm22["PROC_TIME"].ToString());

			//计算吹氩时间
			tmmsm22["AR_BLOW_TIME"] = 0;
			if ("" != tmmsm22["AR_START_TIME"].ToString().Trim() && "" != tmmsm22["AR_END_TIME"].ToString().Trim())
			{
				CDateTime startDateTime = CDateTime::Parse(tmmsm22["AR_START_TIME"].ToString());
				CDateTime endDateTime = CDateTime::Parse(tmmsm22["AR_END_TIME"].ToString());
				CTimeSpan timeSpan = endDateTime - startDateTime;
				CDecimal diffTime = timeSpan.TotalMinutes();
				if (0 >= diffTime){
					strcpy(s.msg, "吹氩时间需大于0，请输入正确的吹氩开始时刻、吹氩结束时刻。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm22["AR_BLOW_TIME"] = diffTime;
			}
			Log::Trace("", __FUNCTION__, "tmmsm22[AR_BLOW_TIME]=[{0}]", tmmsm22["AR_BLOW_TIME"].ToString());

			tmmsm22.Insert();
			v_pract_rcv_flag = "1";
			//计算钢包温降（钢包温降 = 转炉出钢温度 - 吹氩站钢水进站温度）
			v_ladle_down_temp = 0;
			//更新钢包累计盛钢量、钢包温降
			ttmsm01["LOAD_STEEL_WT_TOTAL"] = ttmsm01["LOAD_STEEL_WT_TOTAL"].ToDecimal() + tmmsm22["STEEL_WT"].ToDecimal();
			ttmsm01.Update("LOAD_STEEL_WT_TOTAL", "LADLE_NO");
			//if (ttmsm01["HEAT_NO"].ToString() == tmmsm22["HEAT_NO"].ToString()){
			//	ttmsm01["LADLE_DOWN_TEMP"] = v_ladle_down_temp;
			//	ttmsm01.Update("LADLE_DOWN_TEMP", "LADLE_NO,HEAT_NO");
			//}
			//else
			//{
			//	htmsm01["LADLE_NO"] = tmmsm22["LADLE_NO"];
			//	htmsm01["HEAT_NO"] = tmmsm22["HEAT_NO"];
			//	htmsm01["LADLE_DOWN_TEMP"] = v_ladle_down_temp;
			//	htmsm01.Update("LADLE_DOWN_TEMP", "LADLE_NO,HEAT_NO");
			//}
			//更新转炉钢水重量
			tmmsm21["HEAT_NO"] = tmmsm22["HEAT_NO"];
			tmmsm21["STEEL_NET_WT"] = tmmsm22["STEEL_WT"];
			tmmsm21.Update("STEEL_NET_WT", "HEAT_NO");
			//更新计划出钢钢水重量
			tpssm11["HEAT_NO"] = tmmsm22["HEAT_NO"];
			tpssm11["OUT_STEEL_WT"] = tmmsm22["STEEL_WT"];   
			tpssm11["AR_START_TIME"] = tmmsm22["AR_START_TIME"];
			tpssm11["AR_END_TIME"] = tmmsm22["AR_END_TIME"];
			tpssm11.Update("OUT_STEEL_WT,AR_START_TIME,AR_END_TIME", "HEAT_NO");
		}
		else if (v_proc_div == "U")
		{
			tmmsm22_old.CopyFrom(tmmsm22);
			if (!tmmsm22_old.Query("HEAT_NO"))
			{
				strcpy(s.msg, "熔炼号：" + tmmsm22["HEAT_NO"].ToString() + " 吹氩实绩信息查询失败。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm22["PROC_NO"] = tmmsm22["HEAT_NO"];
			if ("" == tmmsm22["START_TIME"].ToString().Trim() || "" == tmmsm22["END_TIME"].ToString().Trim())
			{
				strcpy(s.msg, "熔炼号：" + tmmsm22["LADLE_NO"].ToString() + " 进站时刻、出站时刻不能为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//计算处理时间
			tmmsm22["PROC_TIME"] = 0;
			if ("" != tmmsm22["START_TIME"].ToString().Trim() && "" != tmmsm22["END_TIME"].ToString().Trim())
			{
				CDateTime startDateTime = CDateTime::Parse(tmmsm22["START_TIME"].ToString());
				CDateTime endDateTime = CDateTime::Parse(tmmsm22["END_TIME"].ToString());
				CTimeSpan timeSpan = endDateTime - startDateTime;
				CDecimal diffTime = timeSpan.TotalMinutes();
				if (0 >= diffTime){
					strcpy(s.msg, "吹氩处理时间需大于0，请输入正确的进站时刻、出站时刻。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm22["PROC_TIME"] = diffTime;
			}
			Log::Trace("", __FUNCTION__, "tmmsm22[PROC_TIME]=[{0}]", tmmsm22["PROC_TIME"].ToString());

			//计算吹氩时间
			tmmsm22["AR_BLOW_TIME"] = 0;
			if ("" != tmmsm22["AR_START_TIME"].ToString().Trim() && "" != tmmsm22["AR_END_TIME"].ToString().Trim()){
				CDateTime startDateTime = CDateTime::Parse(tmmsm22["AR_START_TIME"].ToString());
				CDateTime endDateTime = CDateTime::Parse(tmmsm22["AR_END_TIME"].ToString());
				CTimeSpan timeSpan = endDateTime - startDateTime;
				CDecimal diffTime = timeSpan.TotalMinutes();
				if (0 >= diffTime){
					strcpy(s.msg, "吹氩时间需大于0，请输入正确的吹氩开始时刻、吹氩结束时刻。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm22["AR_BLOW_TIME"] = diffTime;
			}
			Log::Trace("", __FUNCTION__, "tmmsm22[AR_BLOW_TIME]=[{0}]", tmmsm22["AR_BLOW_TIME"].ToString());
			
			tmmsm22.Delete("HEAT_NO");
			tmmsm22.Insert();
			v_pract_rcv_flag = "1";
			if (tmmsm22_old["LADLE_NO"].ToString() != tmmsm22["LADLE_NO"].ToString())
			{
				//更新钢包累计盛钢量
				ttmsm01["LOAD_STEEL_WT_TOTAL"] = ttmsm01["LOAD_STEEL_WT_TOTAL"].ToDecimal() + tmmsm22["STEEL_WT"].ToDecimal();
				ttmsm01.Update("LOAD_STEEL_WT_TOTAL", "LADLE_NO");
				
				//校验钢包
				ttmsm01["LADLE_NO"] = tmmsm22_old["LADLE_NO"];
				if (!ttmsm01.Query("LADLE_NO"))
				{
					strcpy(s.msg, "钢包号：" + ttmsm01["LADLE_NO"].ToString() + " 信息查询失败(修改前钢包号)。");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				ttmsm01["LOAD_STEEL_WT_TOTAL"] = ttmsm01["LOAD_STEEL_WT_TOTAL"].ToDecimal() - tmmsm22_old["STEEL_WT"].ToDecimal();
				ttmsm01.Update("LOAD_STEEL_WT_TOTAL", "LADLE_NO");
			}
			else
			{
				if (tmmsm22_old["STEEL_WT"].ToDecimal() != tmmsm22["STEEL_WT"].ToDecimal()){
					//更新钢包累计盛钢量
					ttmsm01["LOAD_STEEL_WT_TOTAL"] = ttmsm01["LOAD_STEEL_WT_TOTAL"].ToDecimal() - tmmsm22_old["STEEL_WT"].ToDecimal() + tmmsm22["STEEL_WT"].ToDecimal();
					ttmsm01.Update("LOAD_STEEL_WT_TOTAL", "LADLE_NO");
					//更新转炉钢水重量
					tmmsm21["HEAT_NO"] = tmmsm22["HEAT_NO"];
					tmmsm21["STEEL_NET_WT"] = tmmsm22["STEEL_WT"];
					tmmsm21.Update("STEEL_NET_WT", "HEAT_NO");
					//更新计划出钢钢水重量
					tpssm11["HEAT_NO"] = tmmsm22["HEAT_NO"];
					tpssm11["OUT_STEEL_WT"] = tmmsm22["STEEL_WT"];
					tpssm11["AR_START_TIME"] = tmmsm22["AR_START_TIME"];
					tpssm11["AR_END_TIME"] = tmmsm22["AR_END_TIME"];
					tpssm11.Update("OUT_STEEL_WT,AR_START_TIME,AR_END_TIME", "HEAT_NO");
				}
			}
		}
		else if (v_proc_div == "D")
		{
			if (!tmmsm22.Query("HEAT_NO"))
			{
				strcpy(s.msg, "熔炼号：" + tmmsm22["HEAT_NO"].ToString() + " 吹氩实绩信息查询失败。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tmmsm22.Delete("HEAT_NO");
			//更新钢包累计盛钢量
			ttmsm01["LOAD_STEEL_WT_TOTAL"] = ttmsm01["LOAD_STEEL_WT_TOTAL"].ToDecimal() - tmmsm22["STEEL_WT"].ToDecimal();
			ttmsm01.Update("LOAD_STEEL_WT_TOTAL", "LADLE_NO");

			tmmsm2a["HEAT_NO"] = tmmsm22["HEAT_NO"];
			tmmsm2b["HEAT_NO"] = tmmsm22["HEAT_NO"];
			tmmsm2a["PROC_NO"] = tmmsm22["PROC_NO"];
			tmmsm2b["PROC_NO"] = tmmsm22["PROC_NO"];

			blkNum = bcls_rec->Tables.IndexOf("MMSM53");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("MMSM53");
			}


			if (!bcls_rec->Tables["MMSM53"].Columns.Contains("PROC_DIV"))
			{
				bcls_rec->Tables["MMSM53"].Columns.Add(DT_STRING, "PROC_DIV");
			}

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:

				sqlstr = " SELECT * FROM TMMSM2A "
					"  WHERE PROC_NO = @proc_no ";

				break;
			}

			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.Parameters.Set("proc_no", tmmsm22["PROC_NO"].ToString());
			cmd_sql.ExecuteReader();

			while (cmd_sql.Read())
			{
				cmd_sql.Fetch(tmmsm2a);

				bcls_rec->Tables["MMSM53"].Rows.Clear();
				tmmsm2a.MergeTo(bcls_rec->Tables["MMSM53"], false);
				bcls_rec->Tables["MMSM53"].Rows[0]["PROC_DIV"] = v_proc_div;

				doFlag = f_mmsm53_proc(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}

			}
			cmd_sql.Close();

			tmmsm2a.Delete("HEAT_NO,PROC_NO");
			tmmsm2b.Delete("HEAT_NO,PROC_NO");

			v_pract_rcv_flag = " ";

		}
	
		/****** 调用炼钢实绩总表处理函数 ***** */
		if (!bcls_rec->Tables.Contains("MMSM10"))
		{
			bcls_rec->Tables.Add("MMSM10");
		}

		//炉号
		if (!bcls_rec->Tables["MMSM10"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["MMSM10"].Columns.Add(DT_STRING, "HEAT_NO");
		}

		if (!bcls_rec->Tables["MMSM10"].Columns.Contains("PROC_NO"))
		{
			bcls_rec->Tables["MMSM10"].Columns.Add(DT_STRING, "PROC_NO");
		}

		//工位标识
		if (!bcls_rec->Tables["MMSM10"].Columns.Contains("STATION_ID"))
		{
			bcls_rec->Tables["MMSM10"].Columns.Add(DT_STRING, "STATION_ID");
		}

		if (!bcls_rec->Tables["MMSM10"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["MMSM10"].Columns.Add(DT_STRING, "PROC_DIV");
		}
		if (!bcls_rec->Tables["MMSM10"].Columns.Contains("PONO"))
		{
			bcls_rec->Tables["MMSM10"].Columns.Add(DT_STRING, "PONO");
		}
		if (!bcls_rec->Tables["MMSM10"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["MMSM10"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}

		bcls_rec->Tables["MMSM10"].Rows.Add();
		bcls_rec->Tables["MMSM10"].Rows[0]["HEAT_NO"] = tmmsm22["HEAT_NO"];
		bcls_rec->Tables["MMSM10"].Rows[0]["PROC_NO"] = tmmsm22["PROC_NO"];
		bcls_rec->Tables["MMSM10"].Rows[0]["STATION_ID"] = tmmsm00["STATION_ID"];
		bcls_rec->Tables["MMSM10"].Rows[0]["PONO"] = tmmsm22["PONO"];
		bcls_rec->Tables["MMSM10"].Rows[0]["L2_PROC_NO"] = tmmsm22["L2_PROC_NO"];
		bcls_rec->Tables["MMSM10"].Rows[0]["PROC_DIV"] = v_proc_div;

		doFlag = f_mmsm10_trace(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		
		
		#if  defined _SYS_MES   || defined _SYS_PES
		/****** 调用炼钢计划函数 ***** */
		if(!bcls_rec->Tables.Contains("PSSM12"))
		{
			bcls_rec->Tables.Add("PSSM12");
		}

		//实绩接受标记
		if(!bcls_rec->Tables["PSSM12"].Columns.Contains("PRACT_RCV_FLAG"))
		{
			bcls_rec->Tables["PSSM12"].Columns.Add(DT_STRING,"PRACT_RCV_FLAG"); 
		}


		tmmsm00.MergeTo(bcls_rec->Tables["PSSM12"], false);
		bcls_rec->Tables["PSSM12"].Rows[0]["PRACT_RCV_FLAG"] = v_pract_rcv_flag;
		//梅钢吹氩站无TPSSM12
		//doFlag = f_pssm12_mm_rcv(bcls_rec, bcls_ret, conn);		
		//if(doFlag < 0)
		//{
		//	throw CApplicationException(-1, s.msg, log.Location); 
		//}
		#endif
	

		#if defined(_SYS_PES)
		//调用发送电文
		blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
		if(blkNum < 0)
		{
				bcls_rec->Tables.Add("MMSMSND"); 
		}

		if(!bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
		}
	
		if(!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING,"PROC_DIV");
		}

		if(!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_NO"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING,"PROC_NO");
		}
		if(!bcls_rec->Tables["MMSMSND"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING,"HEAT_NO");
		}

		bcls_rec->Tables["MMSMSND"].Rows.Add();
		bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM22";
		bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tmmsm22["PROC_NO"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tmmsm22["HEAT_NO"];
							
		//doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
		if(doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		#endif

			

		/*设置系统返回参数*/
		strcpy(s.msg,  _RES("GCRSS0000002"));//处理成功。  

	}



	/*捕获数据库操作异常*/
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		//LogTrace(1,1,"%s",(const char*)sqlstr);
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = "DB error:" + sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		 
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

	//LogTrace(1,1,"doFlag[%d]s.msg[%s],s.sysmsg[%s]",doFlag,s.msg,s.sysmsg);
	////LogTrace(1, 1, " **************%s end*****************", (const char*)FunctionEname);
	s.flag = doFlag;
	bcls_ret->SetSYS(s);
	return doFlag;

} 

