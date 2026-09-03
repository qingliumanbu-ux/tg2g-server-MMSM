/*************************************************
Copyright:Baosight Software LTD.co Copyright (c) 2010
Author:向萍
Date:2015-06-13
Version:1.0
Description: 接收PES钢种判定电文
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
#include "epex.h"
/***** C++ 的业务头文件部分 *****/ 



/* ***** 静态函数申明 ***** */

int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn); 

#if defined _LINE_HP       //定义有厚板产线
int f_pshp_dhcr_del(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//厚板DHCR计划材料删除函数
int f_qmtqhp_dele_resv_chg_new(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

/*<remark>=========================================================
/// <summary>
/// 接收PES钢种判定电文
/// <para>
/// 接收PES钢种判定电文并处理
/// </para>
/// </summary>
/// <param name="MAT_NO">材料号</param>
/// <param name="EVENT_ID">事件号</param>
/// <returns>处理结果</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE_TELE(cm_200002_rcv)


int f_cm_200002_rcv(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	int i=0;


	/* 业务变量 */
	CString	datetime("");
	CString v_event_id("");
	CString v_fin_st_no("");
	CString v_st_no("");
	CString v_mat_no("");
	CString v_pono_slab("");
	CString s_pono_slab = " ";//调用计划删除的命令板坯号
	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;
	EIClass bcls_rec_dele_resv;          //
	EIClass bcls_ret_dele_resv;          //


	/* 数据库操作类定义 */
	CDbCommand cmd_sql(conn);

	try
	{

		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			v_mat_no= bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("FIN_ST_NO"))
			v_fin_st_no = bcls_rec->Tables[0].Rows[0]["FIN_ST_NO"].ToString().TrimOrBlank().ToUpper();
		


		////Log::Info("", __FUNCTION__,  "v_mat_no=[{0}], v_fin_st_no=[{1}]", v_mat_no, v_fin_st_no);

		if (!bcls_rec->Tables.Contains("PSHP"))
		{
			bcls_rec->Tables.Add("PSHP");
			bcls_rec->Tables["PSHP"].Columns.Add(DT_STRING, "PREC_SLAB_NO"); //预定板坯号。
			bcls_rec->Tables["PSHP"].Columns.Add(DT_STRING, "EVENT_ID"); //事件号 = PM
			bcls_rec->Tables["PSHP"].Columns.Add(DT_STRING, "FUNC_ID"); //当前函数。
			bcls_rec->Tables["PSHP"].Rows.Add();
		}

		/* 新增块名 */
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("MM0099"); 
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_LINE_TYPE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"EVENT_DESC");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"SYSTEM_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"FUNC_ID");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"MAT_NO");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"ST_NO");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"FIN_ST_NO");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"REMAIN_CAUSE_CODE");
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING,"REMAIN_REMARK");
		}
		blkNum = bcls_rec->Tables.IndexOf("TMMSM01");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("TMMSM01");
			
		}


		////Log::Info("", __FUNCTION__, "v_st_no1111=[{0}]", tmmsm01["ST_NO"].ToString());


		blkNum = bcls_rec_dele_resv.Tables.IndexOf("QMZSBlock");
		if (blkNum <= 0)
		{
			bcls_rec_dele_resv.Tables.Add("QMZSBlock");
			bcls_rec_dele_resv.Tables["QMZSBlock"].Columns.Add(DT_STRING, "PONO_SLAB");
			bcls_rec_dele_resv.Tables["QMZSBlock"].Columns.Add(DT_STRING, "PONO");
			bcls_rec_dele_resv.Tables["QMZSBlock"].Columns.Add(DT_STRING, "EVENT_ID");
			bcls_rec_dele_resv.Tables["QMZSBlock"].Columns.Add(DT_STRING, "EVENT_DESC");
			bcls_rec_dele_resv.Tables["QMZSBlock"].Columns.Add(DT_STRING, "SYSTEM_ID");
			bcls_rec_dele_resv.Tables["QMZSBlock"].Columns.Add(DT_STRING, "FUNC_ID");
			bcls_rec_dele_resv.Tables["QMZSBlock"].Columns.Add(DT_STRING, "FORM_CODE");
			bcls_rec_dele_resv.Tables["QMZSBlock"].Columns.Add(DT_STRING, "MAT_KIND");
			bcls_rec_dele_resv.Tables["QMZSBlock"].Columns.Add(DT_STRING, "MAT_NO");


		}

		////Log::Info("", __FUNCTION__, "v_st_no22222=[{0}]", tmmsm01["ST_NO"].ToString());

		if (v_mat_no.Trim() == "")
		{
			sprintf(s.msg, "材料号不能为空.");
			throw CApplicationException(-1, s.msg, log.Location); 
		}

			//如果最终出钢记号不等于预定出钢记号，自动脱合同
		tmmsm01["MAT_NO"] = v_mat_no;

		if (tmmsm01.Query("MAT_NO") == false)
		{
			sprintf(s.msg, "材料号不存在.");
			throw CApplicationException(-1, s.msg, log.Location);
		}
			//sqlstr = "SELECT ST_NO "
			//				 "  FROM TMMSM01 "
			//				 " WHERE MAT_NO	= @mat_no";
			//
			//cmd_sql.SetCommandText(sqlstr);
			//cmd_sql.Parameters.Clear();
			//cmd_sql.Parameters.Set("mat_no",v_mat_no); 
			//cmd_sql.ExecuteReader();
			//if(cmd_sql.Read())                                            
			//{
			//	v_st_no	= cmd_sql.GetString(1);
		
			//}
			//cmd_sql.Close();
		v_st_no = tmmsm01["ST_NO"];
		////Log::Info("", __FUNCTION__,  "v_st_no=[{0}]", tmmsm01["ST_NO"].ToString());

		tmmsm01.MergeTo(bcls_rec->Tables["TMMSM01"],false );
		if(v_fin_st_no.Trim() != v_st_no)
		{
		#if defined _LINE_HP    
			if (tmmsm01["MAT_DESTION"].ToString() == "10") //厚板
			{
				for (int j = 1; j <= 12; j++)
				{
					//Log::Trace("", __FUNCTION__, "pssm03_count000000 = [{0}]", j);
					if (bcls_rec->Tables["TMMSM01"].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)].ToString().Trim() == "")
						break;
					v_pono_slab = bcls_rec->Tables["TMMSM33"].Rows[i]["PONO_SLAB_" + CConvert::ToString(j)];
					//Log::Trace("", __FUNCTION__, "v_pono_slab = [{0}]", v_pono_slab);

					bcls_rec_dele_resv.Tables["QMZSBlock"].Rows.Clear();
					bcls_rec_dele_resv.Tables["QMZSBlock"].Rows.Add();
					bcls_rec_dele_resv.Tables["QMZSBlock"].Rows[0]["PONO_SLAB"] = v_pono_slab;
					bcls_rec_dele_resv.Tables["QMZSBlock"].Rows[0]["PONO"] = tmmsm01["PONO"];
					bcls_rec_dele_resv.Tables["QMZSBlock"].Rows[0]["EVENT_ID"] = "QM70";
					bcls_rec_dele_resv.Tables["QMZSBlock"].Rows[0]["EVENT_DESC"] = "炉次钢种判定";
					bcls_rec_dele_resv.Tables["QMZSBlock"].Rows[0]["SYSTEM_ID"] = "MMSM";
					bcls_rec_dele_resv.Tables["QMZSBlock"].Rows[0]["FUNC_ID"] = "cm_200002_rcv";
					bcls_rec_dele_resv.Tables["QMZSBlock"].Rows[0]["FORM_CODE"] = s.formname;
					bcls_rec_dele_resv.Tables["QMZSBlock"].Rows[0]["MAT_KIND"] = "SM";
					bcls_rec_dele_resv.Tables["QMZSBlock"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];



					doFlag = f_qmtqhp_dele_resv_chg_new(&bcls_rec_dele_resv, &bcls_ret_dele_resv, conn);
				}
		
				
			}
		#endif



			bcls_rec->Tables["MM0099"].Rows.Clear();
			bcls_rec->Tables["MM0099"].Rows.Add();  
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"]			= "QM16";
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"]	= "00"; 
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"]			= "炉次钢种判定";
			bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"]			= "MMSM"; 
			bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"]			= "cm_200002_rcv"; 
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"]			= v_mat_no; 
			bcls_rec->Tables["MM0099"].Rows[0]["REMAIN_CAUSE_CODE"]		= "0270"; 
			bcls_rec->Tables["MM0099"].Rows[0]["REMAIN_REMARK"]				= "质量封锁脱合同"; 
	
			doFlag = f_mmsm99(bcls_rec, bcls_ret,conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}


		
#if defined _LINE_HP       
			    //定义有厚板产线
				//如果是长坯，会对应多个命令板坯，所以需要读目的板坯表后循环处理
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:
					sqlstr = " SELECT PONO_SLAB "
						"   FROM TMMSM03 "
						"  WHERE MAT_NO =  @MAT_NO"
						" AND PONO_SLAB !=' '  "
						"  ORDER BY PONO_SLAB ASC ";
					break;
				}
				cmd_sql.SetCommandText(sqlstr);
				cmd_sql.Parameters.Set("MAT_NO", tmmsm01["MAT_NO"]);
				cmd_sql.ExecuteReader();
				while (cmd_sql.Read())
				{
					s_pono_slab = cmd_sql.GetString(1).TrimOrBlank();
					Log::Trace("", "", "s_pono_slab[{0}]", s_pono_slab);

					bcls_rec->Tables["PSHP"].Rows[0]["PREC_SLAB_NO"] = s_pono_slab;//虚拟材料号。
					bcls_rec->Tables["PSHP"].Rows[0]["EVENT_ID"] = "PM"; //PSHP，要求PM写死，即可。
					bcls_rec->Tables["PSHP"].Rows[0]["FUNC_ID"] = "qmtj_deal"; //当前函数。
					doFlag = f_pshp_dhcr_del(bcls_rec, bcls_ret, conn);
					if (doFlag != 0)
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				cmd_sql.Close();
#endif
				

		}

		
		/* 主材料抛物料跟踪履历 */
		bcls_rec->Tables["MM0099"].Rows.Clear();
		bcls_rec->Tables["MM0099"].Rows.Add();  
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"]			= "QM70";
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"]	= "00"; 
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"]			= "炉次钢种判定";
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"]			= "MMSM"; 
		bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"]			= "cm_200002_rcv"; 
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"]			= v_mat_no; 
		bcls_rec->Tables["MM0099"].Rows[0]["FIN_ST_NO"]		= v_fin_st_no; 
		bcls_rec->Tables["MM0099"].Rows[0]["ST_NO"]				= v_fin_st_no; 
	
		doFlag = f_mmsm99(bcls_rec, bcls_ret,conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}


	




		////判断是否已经有二切材料产生
		//switch(conn->DatabaseKind)
		//{
		//		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//		case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		//		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//		case DB_KIND_ORACLE:	    // Oracle 数据库
		//		default:
		//			sqlstr = "SELECT MAT_LINE_TYPE "
		//					 "  FROM TMMSM01 "
		//					 " WHERE MAT_NO	= @tmmsm96.MAT_NO";
		//		break;
		//	}  
		//	cmd_sql.SetCommandText(sqlstr);
		//	cmd_sql.Parameters.Clear();
		//	cmd_sql.Parameters.Set("tmmsm96.MAT_NO",tmmsm96.MAT_NO); 
		//	cmd_sql.ExecuteReader();
		//	if(cmd_sql.Read())                                            
		//	{
		//		v_mat_line_type	= cmd_sql.GetString(1);
		//	}
		//	cmd_sql.Close();

		//	if (v_mat_line_type.Trim() == "HP")
		//	{
		//		switch(conn->DatabaseKind)
		//		{
		//				case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		//				case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
		//				case DB_KIND_MSSQL:	        // MS SQL Server数据库
		//				case DB_KIND_ORACLE:	    // Oracle 数据库
		//				default:
		//					sqlstr = "SELECT MAT_NO "
		//							 "  FROM TMMSM01 "
		//							 " WHERE IN_MAT_NO	= @tmmsm96.MAT_NO";
		//				break;
		//			}  
		//			cmd_sql.SetCommandText(sqlstr);
		//			cmd_sql.Parameters.Clear();
		//			cmd_sql.Parameters.Set("tmmsm96.MAT_NO",tmmsm96.MAT_NO); 
		//			cmd_sql.ExecuteReader();
		//			while(cmd_sql.Read())                                            
		//			{
		//				v_mat_no	= cmd_sql.GetString(1);

		//				bcls_rec->Tables["MM0099"].Rows.Clear();
		//				bcls_rec->Tables["MM0099"].Rows.Add();  
		//				bcls_rec->Tables["MM0099"].Rows[i]["EVENT_ID"]			= "QM71";
		//				bcls_rec->Tables["MM0099"].Rows[i]["EVENT_LINE_TYPE"]	= "00"; 
		//				bcls_rec->Tables["MM0099"].Rows[i]["SYSTEM_ID"]			= "MMSM"; 
		//				bcls_rec->Tables["MM0099"].Rows[i]["FUNC_ID"]			= "cm_200002_rcv"; 
		//				bcls_rec->Tables["MM0099"].Rows[i]["MAT_NO"]			= v_mat_no; 
		//				bcls_rec->Tables["MM0099"].Rows[i]["JUDGE_ST_NO"]	= tmmsm96.JUDGE_ST_NO; 
		//				bcls_rec->Tables["MM0099"].Rows[i]["FIN_ST_NO"]		= tmmsm96.FIN_ST_NO; 
		//				bcls_rec->Tables["MM0099"].Rows[i]["ST_NO"]				= tmmsm96.ST_NO; 
		//				i = i + 1;

		//			}
		//			cmd_sql.Close();

		//	

		//	}
		 

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
