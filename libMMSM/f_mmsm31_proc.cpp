/***********************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: CC实绩增删改
*************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

 
 
 
 



//外部函数声明
int f_tmsm_cc01(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_qmts_yc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); 
int f_mmsm_sj_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);
int f_mmsm10_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_acyfl_tlcc(EIClass * bcls_rec, EIClass * bcls_ret, CString& v_acjc_relation_id, CDbConnection * conn);
int f_mmsm_acyfl_seq(CString& v_acjc_relation_id, CDbConnection * conn);
int f_mmsm3331_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_t8f011_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源
int f_t823sa_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量


#if  defined _SYS_MES   || defined _SYS_PES
int f_pssm11_cut_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_pssm12_mm_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

#if  defined _SYS_PES
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//L4
#endif

int f_mmsm009c_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//存表到集控大屏
int f_mmsm_gyins2(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//原料函数
int f_t82304_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发给智慧质量的工艺路径
int f_t8z_23m_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//发送专家系统数据


BM2_FUNCTION_EXPORT
int f_mmsm31_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	/****** 定义函数名称 ***** */
CString FunctionEname = "f_mmsm31_proc";                //定义函数英文名称  
CString FunctionCname = "CC信息增删改";          //定义函数中文名称


CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
  

  //程序用变量
	int   doFlag = 0;
	int   fetchRowCount = 0;
	int   i = 0;
	int   blkNum;
	EIClass mmsmgy06;
	EIClass mmsm2304;

	CString sqlstr="";
	CString v_proc_div= "";
	CString v_pract_rcv_flag= "";
	CDecimal  v_cut_slab = 0;
	CDecimal  v_cut_charge = 0;
	CString v_factory_div = "";
	CString v_station_id = "";
	CString v_heat_no = "";
	CString v_acjc_relation_id = "";
	CDbCommand cmd_inq(conn);

    CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
			
	EIClass inBlock1;
	EIClass outBlock1;

     
  try
  {
		CPageInfo pageInfo;	

		/*数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。
		CDbCommand cmd_inq(conn); //与DB 建立连接。
		CDbCommand cmd_id(conn);

		CString PROD_SHIFT_NO = "";
		CString PROD_SHIFT_GROUP = "";
		/* 实体类定义 */
	CModel tmmsm31("TMMSM31");
	CModel tmmsm31_old("TMMSM31");
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm2b("TMMSM2B");
	CModel tmmsm00("TMMSM00");
	CModel tmmsmt8f0("TMMSMT8F0");

	EIClass in_23m;
	in_23m.Tables[0].Columns.Add(DT_STRING, "TC_NO");
	in_23m.Tables[0].Rows.Add();
	in_23m.Tables[0].Rows[0]["TC_NO"] = "T82321";
	in_23m.Tables.Add();
	in_23m.Tables[1].Columns.Add(tmmsm31);
			
    //初始化实体类
	
		tmmsm31.Reset();
		tmmsm2a.Reset();
		tmmsm2b.Reset();
		tmmsm00.Reset();

		//调用校验及公共处理函数,针对有共性的字段进行赋值

		if(!bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING,"TABLE_TYPE"); 
		}
	
		bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"] = "TMMSM31";

		doFlag = f_mmsm_sj_proc(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		tmmsm00.MergeFrom(bcls_ret->Tables["TMMSM00"].Rows[0]);
		tmmsm00["STATION_ID"] = "C";
		tmmsm00.TrimOrBlank();


		


		/*如果是电文调用需要在电文接收service里对厂别和设备类型进行赋值*/
		if (bcls_rec->Tables[0].Columns.Contains("PROC_DIV"))  //处理区分  I-新增  U-修改  D-删除
			v_proc_div = bcls_rec->Tables[0].Rows[0]["PROC_DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))  //厂别
			v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("STATION_ID"))  //设备类型
			v_station_id = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString().TrimOrBlank().ToUpper();

		//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
		//Log::Trace("", __FUNCTION__, "v_factory_div=[{0}]", v_factory_div);
		//Log::Trace("", __FUNCTION__, "v_station_id=[{0}]", v_station_id);
		
		tmmsm31.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm31.TrimOrBlank();
		if (tmmsm31["L2_PROC_NO"].ToString() == " "){
			tmmsm31["L2_PROC_NO"] = tmmsm31["PROC_NO"];
		}
		if (tmmsm31["PROC_NO"].ToString() == " "){
			tmmsm31["PROC_NO"] = tmmsm31["L2_PROC_NO"];
		}
		tmmsm31["FACTORY_DIV"] = "LG1";
		tmmsm00["FACTORY_DIV"] = "LG1";
		tmmsm31.CopyFrom(tmmsm00);

		/*//Log::Trace("", __FUNCTION__, "tmmsm31["LADLE_ARRIVE_WT"] =[{0}]", tmmsm31["LADLE_ARRIVE_WT"].ToDecimal());
		//Log::Trace("", __FUNCTION__, "tmmsm31["LADLE_LEAVE_WT"] =[{0}]", tmmsm31["LADLE_LEAVE_WT"].ToDecimal());

		if (tmmsm31["LADLE_ARRIVE_WT"].ToDecimal() - tmmsm31["LADLE_LEAVE_WT"].ToDecimal() == 0)
		{
			strcpy(s.msg, "浇注钢水重量不能为0,钢包离开重量和钢包到达重量不能相同!");
			throw CApplicationException(-1, s.msg, log.Location);
		}*/


		//生产日期暂时定为取开始时刻。  mfj  20231120
		if (tmmsm31["START_TIME"].ToString().Trim() != "" && tmmsm31["PROD_DATE"].ToString().Trim() == "")
		{
			tmmsm31["PROD_DATE"] = tmmsm31["START_TIME"].ToString().SubstringNE(0, 8);
		}

		if (tmmsm31["PROD_DATE"].ToString().Trim() != "")
		{
			tmmsm31["PROD_DATE"] = tmmsm31["PROD_DATE"].ToString().Substring(0, 8);
		}
		Log::Trace("", "", "PROD_DATE={0}", tmmsm31["PROD_DATE"].ToString());
		f_mmsm_acyfl_seq(v_acjc_relation_id, conn);
		//抛帐Start
		if (bcls_rec->Tables.IndexOf("MMSMAC") < 0)
		{
			bcls_rec->Tables.Add("MMSMAC");
		}
		bcls_rec->Tables["MMSMAC"].Rows.Add();
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("TABLE_NAME"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "TABLE_NAME");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "HEAT_NO");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("FLAG"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "FLAG");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("ACJC_RELATION_ID"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "ACJC_RELATION_ID");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "PROC_DIV");
		}
		bcls_rec->Tables["MMSMAC"].Rows[0]["TABLE_NAME"] = "TMMSM31";
		bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		bcls_rec->Tables["MMSMAC"].Rows[0]["FLAG"] = "0"; //0抛负数 1抛正数
		bcls_rec->Tables["MMSMAC"].Rows[0]["ACJC_RELATION_ID"] = dateNow + "0000";
		bcls_rec->Tables["MMSMAC"].Rows[0]["PROC_DIV"] = v_proc_div;
		v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		//Log::Trace("", __FUNCTION__, "bcls_rec->MMSMAC->HEAT_NO=[{0}]", bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"].ToString().Trim());
		//doFlag = f_mmsm_acyfl_tlcc(bcls_rec, bcls_ret, v_acjc_relation_id, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//抛帐End
	
		if (tmmsm31["PROD_SHIFT_NO"].ToString().Trim() == "")
		{
			f_epep_get_shift_group("SMDD", tmmsm31["END_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
			tmmsm31["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
		}
		//查询tmmsm31c
		cmd_inq.SetCommandText(" select STEEL_NET_WT_CRANE  from TMMSM31C where HEAT_NO=@HEAT_NO ORDER BY REC_CREATE_TIME DESC ");
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("HEAT_NO",tmmsm31["HEAT_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm31["STEEL_NET_WT_CRANE"] = cmd_inq.GetString(1);

		}
		cmd_inq.Close();
		if(v_proc_div == "I")
		{
			try
			{
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	        // MS SQL Server数据库
				case DB_KIND_ORACLE:        // Oracle 数据库
				default:
					sqlstr = CString(
						" SELECT FIN_ST_NO,JUDGE_CODE FROM TQMTS23 "
						"  WHERE HEAT_NO = @heat_no "
						"    AND JUDGE_CODE = '1' "
						);
					break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("heat_no", v_heat_no);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tmmsm31["FIN_ST_NO"] = cmd_inq.GetString(1);
				}
				else
				{
					tmmsm31["FIN_ST_NO"] = " ";
				}
				cmd_inq.Close();
				//Log::Trace("", "", "获取最终出钢记号={0}", tmmsm31["FIN_ST_NO"].ToString());
				//bcls_rec->Tables["TMMSM33"].Rows[0]["FIN_ST_NO"] = tmmsm31["FIN_ST_NO"];
			}
			catch (CDbException& ex)
			{
				;
			}
			//产品化三明模式，避免弹窗里保存按钮点击多次
			tmmsm31_old["HEAT_NO"] = tmmsm31["HEAT_NO"].ToString();
			tmmsm31_old["PROC_NO"] = tmmsm31["PROC_NO"].ToString();
			if (tmmsm31["ID_SJ"].ToString() == " " || tmmsm31["ID_SJ"].ToString() == ""){
				cmd_id.SetCommandText(" SELECT LPAD(TO_CHAR(TEST_ID_XMY.NEXTVAL), 9, '0') AS ID FROM DUAl ");
				cmd_id.ExecuteReader();
				if (cmd_id.Read())
				{
					tmmsm31["ID_SJ"] = cmd_id.GetString(1);
				}
				cmd_id.Close();
				CString f = "-";
				tmmsm31["ID_SJ"] = f + tmmsm31["ID_SJ"].ToString();
			}
			if (tmmsm31_old.QueryCount("HEAT_NO,PROC_NO") > 0)
			{
				strcpy(s.msg, "主实绩已保存!");
				//throw CApplicationException(-1, s.msg, log.Location);
			}
			else
			{
				tmmsm31.Insert();
				tmmsm31.MergeTo(in_23m.Tables[1]);
			}
			

			//Log::Trace("", "", "获取最终出钢记号11111={0}", tmmsm31["FIN_ST_NO"].ToString());

			blkNum = bcls_rec->Tables.IndexOf("TMMSM31");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("TMMSM31");
				bcls_rec->Tables["TMMSM31"].Columns.Add(DT_STRING, "HEAT_NO");
				bcls_rec->Tables["TMMSM31"].Columns.Add(DT_STRING, "PONO");
			}
			//更新炉次主表
			bcls_rec->Tables["TMMSM31"].Rows.Clear();
			bcls_rec->Tables["TMMSM31"].Rows.Add();
			/*bcls_rec->Tables["TMMSM31"].Rows[0]["HEAT_NO"] = bcls_rec->Tables["TMMSM33"].Rows[0]["HEAT_NO"].ToString();
			bcls_rec->Tables["TMMSM31"].Rows[0]["PONO"] = bcls_rec->Tables["TMMSM33"].Rows[0]["PONO"].ToString();*/
			//doFlag = f_mmsm3331_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//Log::Trace("", "", "获取最终出钢记号22222={0}", tmmsm31["FIN_ST_NO"].ToString());

			v_pract_rcv_flag = "1";

			inBlock1.Tables[0].Columns.Add(DT_STRING,"HEAT_NO");
			inBlock1.Tables[0].Columns.Add(DT_STRING,"WHOLE_BACKLOG_CODE");
 			inBlock1.Tables[0].Columns.Add(DT_STRING,"ST_NO");
			inBlock1.Tables[0].Columns.Add(DT_STRING,"PROC_NO");

			inBlock1.Tables[0].Rows.Add();
			inBlock1.Tables[0].Rows[0]["HEAT_NO"] = tmmsm31["HEAT_NO"];
			inBlock1.Tables[0].Rows[0]["WHOLE_BACKLOG_CODE"] = tmmsm00["STATION_ID"];
			inBlock1.Tables[0].Rows[0]["ST_NO"] = tmmsm31["ST_NO"];
			inBlock1.Tables[0].Rows[0]["PROC_NO"] = tmmsm31["PROC_NO"];
		
			//doFlag = f_qmts_yc(&inBlock1, &outBlock1,conn);
			if(doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location); 
			}
		}
		else if (v_proc_div == "U")
		{
			tmmsm31.Delete(); 
			tmmsm31.Insert();  
			tmmsm31.MergeTo(in_23m.Tables[1]);
			v_pract_rcv_flag = "1";

  		}
		else if (v_proc_div == "D")
		{
			tmmsm2a["HEAT_NO"] = tmmsm31["HEAT_NO"];
			tmmsm2b["HEAT_NO"] = tmmsm31["HEAT_NO"];
			tmmsm31.Delete(); 
			tmmsm2a.Delete("HEAT_NO");
			tmmsm2b.Delete("HEAT_NO");
			v_pract_rcv_flag = " ";
		}


		//抛帐Start
		if (bcls_rec->Tables.IndexOf("MMSMAC") < 0)
		{
			bcls_rec->Tables.Add("MMSMAC");
		}
		bcls_rec->Tables["MMSMAC"].Rows.Add();
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("TABLE_NAME"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "TABLE_NAME");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "HEAT_NO");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("FLAG"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "FLAG");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("ACJC_RELATION_ID"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "ACJC_RELATION_ID");
		}
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "PROC_DIV");
		}
		bcls_rec->Tables["MMSMAC"].Rows[0]["TABLE_NAME"] = "TMMSM31";
		bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		bcls_rec->Tables["MMSMAC"].Rows[0]["FLAG"] = "1"; //0抛负数 1抛正数
		bcls_rec->Tables["MMSMAC"].Rows[0]["ACJC_RELATION_ID"] = dateNow + "0000";
		bcls_rec->Tables["MMSMAC"].Rows[0]["PROC_DIV"] = v_proc_div;
		//Log::Trace("", __FUNCTION__, "bcls_rec->MMSMAC->HEAT_NO=[{0}]", bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"].ToString().Trim());
		//doFlag = f_mmsm_acyfl_tlcc(bcls_rec, bcls_ret, v_acjc_relation_id, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//抛帐End

		//Log::Trace("", __FUNCTION__, "bcls_rec->MMSMAC->HEAT_NO1111111111111=[{0}]", bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"].ToString().Trim());

		//为何要在炉次实绩时判断调用切断完成？？？
		//Log::Trace("", __FUNCTION__, "v_cut_slab111111111111=[{0}]]", v_cut_slab);

		////校验板坯切断标记
		//sqlstr = "SELECT  NVL(SUM(SLAB_NUM),0) "
		//	" FROM    TMMSM33 "
		//	" WHERE   HEAT_NO = @heat_no";
	
		//		
		//cmd_sql.SetCommandText(sqlstr);
		//cmd_sql.Parameters.Clear();
		//cmd_sql.Parameters.Set("heat_no", tmmsm31["HEAT_NO"].ToString());
		//cmd_sql.ExecuteReader();
		//if (cmd_sql.Read())
		//{
		//	v_cut_slab= cmd_sql.GetDecimal(1);
		//}
		//cmd_sql.Close();

		//Log::Trace("", __FUNCTION__, "v_cut_slab=[{0}]]", v_cut_slab);

		//sqlstr = "SELECT  nvl(CUT_SLAB_NUM, 0) "
		//	" FROM    TMMSM31 "
		//	" WHERE   HEAT_NO = @heat_no";
		//			
		//cmd_sql.SetCommandText(sqlstr);
		//cmd_sql.Parameters.Clear();
		//cmd_sql.Parameters.Set("heat_no", tmmsm31["HEAT_NO"].ToString());
		//cmd_sql.ExecuteReader();
		//if (cmd_sql.Read())
		//{
		//	v_cut_charge = cmd_sql.GetDecimal(1);
		//}
		//cmd_sql.Close();

		//Log::Trace("", __FUNCTION__, "v_cut_charge=[{0}]]", v_cut_charge);

		////切断实绩块数》炉次实绩中的块数
		//if (v_cut_slab >= v_cut_charge)
		//{
		//		if(!bcls_rec->Tables.Contains("PSSM11"))
		//		{
		//			bcls_rec->Tables.Add("PSSM11");
		//		}
		//		if(!bcls_rec->Tables["PSSM11"].Columns.Contains("FACTORY_DIV"))
		//		{
		//			bcls_rec->Tables["PSSM11"].Columns.Add(DT_STRING,"FACTORY_DIV");
		//		}
		//		if(!bcls_rec->Tables["PSSM11"].Columns.Contains("HEAT_NO"))
		//		{
		//			bcls_rec->Tables["PSSM11"].Columns.Add(DT_STRING,"HEAT_NO"); 
		//		}
		//		if(!bcls_rec->Tables["PSSM11"].Columns.Contains("CUT_FIN_FLAG"))
		//		{
		//			bcls_rec->Tables["PSSM11"].Columns.Add(DT_STRING,"CUT_FIN_FLAG"); 
		//		}

		//		bcls_rec->Tables["PSSM11"].Rows.Add();
		//		bcls_rec->Tables["PSSM11"].Rows[0]["FACTORY_DIV"] = tmmsm31["FACTORY_DIV"];
		//		bcls_rec->Tables["PSSM11"].Rows[0]["HEAT_NO"] = tmmsm31["HEAT_NO"];
		//		bcls_rec->Tables["PSSM11"].Rows[0]["CUT_FIN_FLAG"] = "1";


		//		doFlag = f_pssm11_cut_rcv(bcls_rec, bcls_ret,conn);		
		//		if(doFlag < 0)
		//		{
		//			throw CApplicationException(-1, s.msg, log.Location); 
		//		}

		//}

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
		bcls_rec->Tables["MMSM10"].Rows[0]["HEAT_NO"] = tmmsm31["HEAT_NO"];
		bcls_rec->Tables["MMSM10"].Rows[0]["STATION_ID"] = tmmsm00["STATION_ID"];
		bcls_rec->Tables["MMSM10"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["MMSM10"].Rows[0]["PONO"] = tmmsm31["PONO"];
		bcls_rec->Tables["MMSM10"].Rows[0]["L2_PROC_NO"] = tmmsm31["L2_PROC_NO"];
		doFlag = f_mmsm10_trace(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

			
        #if  defined _SYS_MES   || defined _SYS_PES
		/****** 调用炼钢计划函数 ***** */
		if (!bcls_rec->Tables.Contains("PSSM12"))
		{
			bcls_rec->Tables.Add("PSSM12");
		}

		//实绩接受标记
		if (!bcls_rec->Tables["PSSM12"].Columns.Contains("PRACT_RCV_FLAG"))
		{
			bcls_rec->Tables["PSSM12"].Columns.Add(DT_STRING, "PRACT_RCV_FLAG");
		}
		if (!bcls_rec->Tables["PSSM12"].Columns.Contains("SM_PLAN_NO"))
		{
			bcls_rec->Tables["PSSM12"].Columns.Add(DT_STRING, "SM_PLAN_NO");
		}

		tmmsm00.MergeTo(bcls_rec->Tables["PSSM12"], false);
		bcls_rec->Tables["PSSM12"].Rows[0]["PRACT_RCV_FLAG"] = v_pract_rcv_flag;
		bcls_rec->Tables["PSSM12"].Rows[0]["SM_PLAN_NO"] = tmmsm31["SM_PLAN_NO"].ToString();
		bcls_rec->Tables["PSSM12"].Rows[0]["FACTORY_DIV"] = "LG1";
		doFlag = f_pssm12_mm_rcv(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
        #endif


		//调用工器具函数
		if (!bcls_rec->Tables.Contains("TMSMCC"))
		{
			bcls_rec->Tables.Add("TMSMCC");
		}

		if (!bcls_rec->Tables["TMSMCC"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["TMSMCC"].Columns.Add(DT_STRING, "PROC_DIV");
		}

		tmmsm31.MergeTo(bcls_rec->Tables["TMSMCC"], false);

		bcls_rec->Tables["TMSMCC"].Rows[0]["PROC_DIV"] = v_proc_div;
	
		//doFlag = f_tmsm_cc01(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//调用发送电文
		Log::Trace("", __FUNCTION__, "集控大屏=[{0}]", v_proc_div);
		blkNum = bcls_rec->Tables.IndexOf("JKDP");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("JKDP");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "PROC_DIV");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("PROC_NO"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "PROC_NO");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "HEAT_NO");
		}

		if (!bcls_rec->Tables["JKDP"].Columns.Contains("TABLE_NAME"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "TABLE_NAME");
		}
		if (!bcls_rec->Tables["JKDP"].Columns.Contains("ID_SJ"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "ID_SJ");
		}
		if (!bcls_rec->Tables["JKDP"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}
		bcls_rec->Tables["JKDP"].Rows.Add();
		bcls_rec->Tables["JKDP"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"] = tmmsm31["PROC_NO"].ToString();
		bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"] = tmmsm31["HEAT_NO"].ToString();
		bcls_rec->Tables["JKDP"].Rows[0]["TABLE_NAME"] = "DA_CCM_PRO_SUMMARY";
		bcls_rec->Tables["JKDP"].Rows[0]["ID_SJ"] = tmmsm31["ID_SJ"].ToString();
		bcls_rec->Tables["JKDP"].Rows[0]["L2_PROC_NO"] = tmmsm31["L2_PROC_NO"].ToString();
		doFlag = f_mmsm009c_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

	
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

		if(!bcls_rec->Tables["MMSMSND"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING,"HEAT_NO");
		}

		if(!bcls_rec->Tables["MMSMSND"].Columns.Contains("PROC_NO"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING,"PROC_NO");
		}
		if(!bcls_rec->Tables["MMSMSND"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING,"L2_PROC_NO");
		}

		bcls_rec->Tables["MMSMSND"].Rows.Add();
		bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM31";
		bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tmmsm31["PROC_NO"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tmmsm31["HEAT_NO"];
		bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tmmsm31["L2_PROC_NO"];
		doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
		if(doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		#endif

		//调用发送电文
		blkNum = bcls_rec->Tables.IndexOf("T8F");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("T8F");
		}

		if (!bcls_rec->Tables["T8F"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "PROC_DIV");
		}

		if (!bcls_rec->Tables["T8F"].Columns.Contains("PROC_NO"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "PROC_NO");
		}

		if (!bcls_rec->Tables["T8F"].Columns.Contains("HEAT_NO"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "HEAT_NO");
		}
		if (!bcls_rec->Tables["T8F"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}
		bcls_rec->Tables["T8F"].Rows.Add();
		bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["T8F"].Rows[0]["PROC_NO"] = tmmsm31["PROC_NO"];
		bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"] = tmmsm31["HEAT_NO"];
		bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"] = tmmsm31["L2_PROC_NO"];
		//T8F011
		doFlag = f_t8f011_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tmmsm31["EMREAD"] = "Y";
		tmmsm31["EMREADTIME"] = dateNow;
		tmmsm31.Update("EMREAD,EMREADTIME", "HEAT_NO,L2_PROC_NO");
		tmmsmt8f0.CopyFrom(tmmsm31);
		tmmsmt8f0.Insert();
		//调用智慧质量发送电文
		if (tmmsm31["HEAT_NO"].ToString() != " "){
			blkNum = bcls_rec->Tables.IndexOf("T823S");
			if (blkNum < 0)
			{
				bcls_rec->Tables.Add("T823S");
			}

			if (!bcls_rec->Tables["T823S"].Columns.Contains("DEAL_FLAG"))
			{
				bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "DEAL_FLAG");
			}

			if (!bcls_rec->Tables["T823S"].Columns.Contains("PROC_NO"))
			{
				bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "PROC_NO");
			}

			if (!bcls_rec->Tables["T823S"].Columns.Contains("HEAT_NO"))
			{
				bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "HEAT_NO");
			}
			if (!bcls_rec->Tables["T823S"].Columns.Contains("L2_PROC_NO"))
			{
				bcls_rec->Tables["T823S"].Columns.Add(DT_STRING, "L2_PROC_NO");
			}
			bcls_rec->Tables["T823S"].Rows.Add();
			bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = v_proc_div;
			bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm31["PROC_NO"];
			bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm31["HEAT_NO"];
			bcls_rec->Tables["T823S"].Rows[0]["L2_PROC_NO"] = tmmsm31["L2_PROC_NO"];
			//doFlag = f_t823sa_snd(bcls_rec, bcls_ret, conn);
		}
		mmsmgy06.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		mmsmgy06.Tables[0].Columns.Add(DT_STRING, "SM_PLAN_NOL2");
		mmsmgy06.Tables[0].Rows.Add();
		mmsmgy06.Tables[0].Rows[0]["HEAT_NO"] = tmmsm31["HEAT_NO"];
		mmsmgy06.Tables[0].Rows[0]["SM_PLAN_NOL2"] = tmmsm31["SM_PLAN_NOL2"];
		doFlag = f_mmsm_gyins2(&mmsmgy06, bcls_ret, conn);

		mmsm2304.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		mmsm2304.Tables[0].Rows.Add();
		mmsm2304.Tables[0].Rows[0]["HEAT_NO"] = tmmsm31["HEAT_NO"];
		doFlag = f_t82304_snd(&mmsm2304, bcls_ret, conn);

		if (in_23m.Tables[1].Rows.get_Count() > 0)
		{
			doFlag = f_t8z_23m_snd(&in_23m, bcls_ret, conn);
		}
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

