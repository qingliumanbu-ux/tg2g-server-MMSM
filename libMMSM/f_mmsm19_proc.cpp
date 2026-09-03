/***********************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-05-25
Description: 中频炉实绩增删改
*************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件

 
 
 
//#include "tmmsm00.h" 



//外部函数声明
int f_mmsm53_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_qmts_yc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); 
int f_mmsm_sj_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_pssm12_mm_rcv(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);
int f_mmsm10_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_acyfl_tlcc(EIClass * bcls_rec, EIClass * bcls_ret, CString& v_acjc_relation_id, CDbConnection * conn);
int f_mmsm_acyfl_seq(CString& v_acjc_relation_id, CDbConnection * conn);
int f_mmsm009c_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//集控大屏写表
int f_t8f009_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//能源
int f_t823s3_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//智慧质量
int f_t823s5_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


BM2_FUNCTION_EXPORT
 int f_mmsm19_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{

	/****** 定义函数名称 ***** */
CString FunctionEname = "f_mmsm19_proc";                //定义函数英文名称  
CString FunctionCname = "转炉信息增删改";          //定义函数中文名称


CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
  

  //程序用变量
  int   doFlag = 0;
  int   fetchRowCount = 0;
  int   i = 0;
  int   blkNum;

  CString sqlstr="";
  CString v_proc_div= "";
  CString v_factory_div = "";
  CString v_station_id = "";
  CString v_pract_rcv_flag= "";
  CString v_acjc_relation_id = "";

  CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
  CDbCommand cmd_inq(conn);
		
  EIClass inBlock1;
  EIClass outBlock1;

     
  try
  {
		CPageInfo pageInfo;	

		/*数据库操作类定义 */
		CDbCommand cmd_sql(conn); //与DB 建立连接。
		CDbCommand cmd_id(conn);
	
		/* 实体类定义 */
	CModel tmmsm19("TMMSM19");
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm2b("TMMSM2B");
	CModel tmmsm00("TMMSM00");
	CModel tmmsmt8f0("TMMSMT8F0");
			
		//初始化实体类
	
		tmmsm19.Reset();
		tmmsm2a.Reset();
		tmmsm2b.Reset();
		tmmsm00.Reset();
		CString PROD_SHIFT_NO = "";
		CString PROD_SHIFT_GROUP = "";


			
		//调用校验及公共处理函数,针对有共性的字段进行赋值009a

	
		if(!bcls_rec->Tables[0].Columns.Contains("TABLE_TYPE"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING,"TABLE_TYPE"); 
		}
		if (!bcls_rec->Tables[0].Columns.Contains("ID_SJ"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "ID_SJ");
		}
		bcls_rec->Tables[0].Rows[0]["TABLE_TYPE"] = "TMMSM19";

		Log::Trace("", __FUNCTION__, "get_Count=[{0}]", bcls_rec->Tables.get_Count());
		Log::Trace("", __FUNCTION__, "HEAT_NO=[{0}]", bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString());

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
			v_station_id= bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString().TrimOrBlank().ToUpper();
		
		//Log::Trace("", __FUNCTION__, "v_proc_div=[{0}]", v_proc_div);
		//Log::Trace("", __FUNCTION__, "v_factory_div=[{0}]", v_factory_div);
		Log::Trace("", __FUNCTION__, "v_station_id=[{0}]", v_station_id);
		
		tmmsm19.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsm19.TrimOrBlank();
		if (tmmsm19["L2_PROC_NO"].ToString() == " "){
			tmmsm19["L2_PROC_NO"] = tmmsm19["PROC_NO"];
		}
		Log::Trace("", __FUNCTION__, "PROC_NO11=[{0}]", tmmsm19["PROC_NO"].ToString());
		tmmsm19["FACTORY_DIV"] = "LG1";
		tmmsm19.CopyFrom(tmmsm00);
		//tmmsm19["PROC_NO"] = tmmsm19["HEAT_NO"];

		//if (tmmsm19.MOLTIRON_WT == 0)
		//	strcpy(s.msg, "铁水重量不能为空!");
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		//tmmsm19.BOTTOM_BLOW_PATTERN = tmmsm19.BOTTOM_BLOW_PATTERN.SubstringNE(0, 5);

		//生产日期暂时定为取开始时刻。  mfj  20231120
		if (tmmsm19["START_TIME"].ToString().Trim() != "" && tmmsm19["PROD_DATE"].ToString().Trim() == "")
		{
			tmmsm19["PROD_DATE"] = tmmsm19["START_TIME"].ToString().SubstringNE(0, 8);
		}
		if (tmmsm19["PROD_DATE"].ToString().Trim() != "")
		{
			tmmsm19["PROD_DATE"] = tmmsm19["PROD_DATE"].ToString().Substring(0, 8);
		}

		


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
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "PROC_DIV");
		}
		bcls_rec->Tables["MMSMAC"].Rows[0]["TABLE_NAME"] = "TMMSM19";
		bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		bcls_rec->Tables["MMSMAC"].Rows[0]["FLAG"] = "0"; //0抛负数 1抛正数
		bcls_rec->Tables["MMSMAC"].Rows[0]["PROC_DIV"] = v_proc_div;
		//Log::Trace("", __FUNCTION__, "bcls_rec->MMSMAC->HEAT_NO=[{0}]", bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"].ToString().Trim());
		doFlag = f_mmsm_acyfl_tlcc(bcls_rec, bcls_ret, v_acjc_relation_id, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//抛帐End
		if (tmmsm19["PROD_SHIFT_NO"].ToString().Trim() == "")
		{
			f_epep_get_shift_group("SMDD", tmmsm19["END_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
			tmmsm19["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
		}

		if(v_proc_div == "I")
		{
			if (tmmsm19["ID_SJ"].ToString() == " " ){
				cmd_id.SetCommandText(" SELECT LPAD(TO_CHAR(TEST_ID_XMY.NEXTVAL), 5, '0') AS ID FROM DUAl ");
				cmd_id.ExecuteReader();
				if (cmd_id.Read())
				{
					tmmsm19["ID_SJ"]= cmd_id.GetDecimal(1);
				}
				cmd_id.Close();
				CString f = "-";
				tmmsm19["ID_SJ"] = f + tmmsm19["ID_SJ"].ToString();
			}
			tmmsm19.Print();
			tmmsm19.Insert();    
			v_pract_rcv_flag = "1";
		
		}
		else if (v_proc_div == "U")
		{
			CString heat_no = " ";
			cmd_inq.SetCommandText(" select HEAT_NO from TMMSM19 WHERE PROC_NO='"+tmmsm19["PROC_NO"].ToString()+"' ");
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				heat_no = cmd_inq.GetString(1);
				if (heat_no == " "){
					tmmsm19.Delete("PROC_NO");
				}
				else
				{
					tmmsm19["HEAT_NO"] = heat_no;
					tmmsm19.Delete("HEAT_NO,PROC_NO");
					tmmsm19["HEAT_NO"] = heat_no;
				}
			}
			cmd_inq.Close();
			tmmsm19.Insert();    
			v_pract_rcv_flag = "1";

  	}
		else if (v_proc_div == "D")
		{
		
			tmmsm19.Delete("HEAT_NO"); 
			
			tmmsm2a["HEAT_NO"] = tmmsm19["HEAT_NO"];
			tmmsm2b["HEAT_NO"] = tmmsm19["HEAT_NO"];
			
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
			cmd_sql.Parameters.Set("proc_no", tmmsm19["HEAT_NO"].ToString());
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
		if (!bcls_rec->Tables["MMSMAC"].Columns.Contains("PROC_DIV"))
		{
			bcls_rec->Tables["MMSMAC"].Columns.Add(DT_STRING, "PROC_DIV");
		}
		bcls_rec->Tables["MMSMAC"].Rows[0]["TABLE_NAME"] = "TMMSM19";
		bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		bcls_rec->Tables["MMSMAC"].Rows[0]["FLAG"] = "1"; //0抛负数 1抛正数
		bcls_rec->Tables["MMSMAC"].Rows[0]["PROC_DIV"] = v_proc_div;
		//Log::Trace("", __FUNCTION__, "bcls_rec->MMSMAC->HEAT_NO=[{0}]", bcls_rec->Tables["MMSMAC"].Rows[0]["HEAT_NO"].ToString().Trim());
		doFlag = f_mmsm_acyfl_tlcc(bcls_rec, bcls_ret, v_acjc_relation_id, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, s.svc_name);
		}
		//抛帐End
	
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
		if (!bcls_rec->Tables["MMSM10"].Columns.Contains("PONO"))
		{
			bcls_rec->Tables["MMSM10"].Columns.Add(DT_STRING, "PONO");
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
		if (!bcls_rec->Tables["MMSM10"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["MMSM10"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}

		bcls_rec->Tables["MMSM10"].Rows.Add();
		bcls_rec->Tables["MMSM10"].Rows[0]["HEAT_NO"] = tmmsm19["HEAT_NO"];
		bcls_rec->Tables["MMSM10"].Rows[0]["PONO"] = tmmsm19["PONO"];
		bcls_rec->Tables["MMSM10"].Rows[0]["STATION_ID"] = v_station_id;
		bcls_rec->Tables["MMSM10"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["MMSM10"].Rows[0]["L2_PROC_NO"] = tmmsm19["L2_PROC_NO"];
		Log::Trace("", __FUNCTION__, "v_station_id=[{0}]", v_station_id);
		//doFlag = f_mmsm10_trace(bcls_rec, bcls_ret, conn);
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
		if (!bcls_rec->Tables["JKDP"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["JKDP"].Columns.Add(DT_STRING, "L2_PROC_NO");
		}

		bcls_rec->Tables["JKDP"].Rows.Add();
		bcls_rec->Tables["JKDP"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["JKDP"].Rows[0]["PROC_NO"] = tmmsm19["PROC_NO"].ToString();
		bcls_rec->Tables["JKDP"].Rows[0]["HEAT_NO"] = tmmsm19["HEAT_NO"].ToString();
		bcls_rec->Tables["JKDP"].Rows[0]["TABLE_NAME"] = "DA_IF_PRO_SUMMARY";
		bcls_rec->Tables["JKDP"].Rows[0]["L2_PROC_NO"] = tmmsm19["L2_PROC_NO"].ToString();
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
		//L2_PROC_NO
		if(!bcls_rec->Tables["MMSMSND"].Columns.Contains("L2_PROC_NO"))
		{
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING,"L2_PROC_NO");
		}
		bcls_rec->Tables["MMSMSND"].Rows.Add();
		bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM19";
		bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["MMSMSND"].Rows[0]["HEAT_NO"] = tmmsm19["HEAT_NO"].ToString();
		bcls_rec->Tables["MMSMSND"].Rows[0]["PROC_NO"] = tmmsm19["PROC_NO"].ToString();
		bcls_rec->Tables["MMSMSND"].Rows[0]["L2_PROC_NO"] = tmmsm19["L2_PROC_NO"].ToString();
		Log::Trace("", __FUNCTION__, "PROC_NO22=[{0}]", tmmsm19["PROC_NO"].ToString());
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
		if (!bcls_rec->Tables["T8F"].Columns.Contains("SM_PLAN_NOL2"))
		{
			bcls_rec->Tables["T8F"].Columns.Add(DT_STRING, "SM_PLAN_NOL2");
		}
		bcls_rec->Tables["T8F"].Rows.Add();
		bcls_rec->Tables["T8F"].Rows[0]["PROC_DIV"] = v_proc_div;
		bcls_rec->Tables["T8F"].Rows[0]["PROC_NO"] = tmmsm19["PROC_NO"].ToString();
		bcls_rec->Tables["T8F"].Rows[0]["HEAT_NO"] = tmmsm19["HEAT_NO"].ToString();
		bcls_rec->Tables["T8F"].Rows[0]["L2_PROC_NO"] = tmmsm19["L2_PROC_NO"].ToString();
		bcls_rec->Tables["T8F"].Rows[0]["SM_PLAN_NOL2"] = tmmsm19["SM_PLAN_NOL2"].ToString();
		doFlag = f_t8f009_snd(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tmmsm19["EMREAD"] = "Y";
		tmmsm19["EMREADTIME"] = dateNow;
		tmmsm19.Update("EMREAD,EMREADTIME","HEAT_NO,L2_PROC_NO");
		tmmsmt8f0.CopyFrom(tmmsm19);
		tmmsmt8f0.Insert();
		//调用智慧质量发送电文
		if (tmmsm19["HEAT_NO"].ToString() != " "){
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

			bcls_rec->Tables["T823S"].Rows.Add();
			bcls_rec->Tables["T823S"].Rows[0]["DEAL_FLAG"] = v_proc_div;
			bcls_rec->Tables["T823S"].Rows[0]["PROC_NO"] = tmmsm19["L2_PROC_NO"].ToString();
			bcls_rec->Tables["T823S"].Rows[0]["HEAT_NO"] = tmmsm19["HEAT_NO"].ToString();
			//doFlag = f_t823s3_snd(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
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

