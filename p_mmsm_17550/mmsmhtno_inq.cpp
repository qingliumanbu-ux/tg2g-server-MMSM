/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    3.0
Date:     2014-05-8
Description: 查询可以做实绩后备的计划信息
**************************************************/

/***** C++ 的标准头文件部分 *****/ 
#include "stdafx.h"
 

/***** C++ 的业务头文件部分 *****/ 
//#include "SQLDDL.h"





int f_mmsm_get_pono(const CString& heat_no, CString& pono, CString& factory_div, CDbConnection * conn);
int f_mmsm_slab_dest(const CString& slab_plan_dest, CString& slab_dest_code, CDbConnection * conn);

/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
///<summary>
///炼钢实绩新增-计划信息查询
///<para>
///1.根据前台传入的条件查询计划信息
///</para>
///<para>数据库表TPSSM12(炼钢作业计划编制子表)</para>
///</summary>
///<param name="FACTORY_DIV">主工序代码</param>
///<param name="STATION_ID">工位代码</param>
///<param name="STATION_NO">工位号</param>
///<returns>指定条件下的计划信息</returns>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(mmsmhtno_inq)

int f_mmsmhtno_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* ***** 自定义变量***** */
	int doFlag = 0;

	int fetchRowCount;
	int blknum;
	int count = 0;
	CString v_factory_div = "";
	CString v_proc_no = "";
	CString v_pono    = "";
	CString v_station_id = "";
	CString v_station_id_up = "";
	CString v_station_no = "";
	CString v_dev_code = "";
	CDecimal  v_area_id = 0;
	CString v_table_name = "";
	CString v_table_name_3 = "";
	CString v_item_name1 = "";
	CString v_item_name2 = "";
	CString v_slab_dest = "";
	CString v_hot_send_flag= "";
	CDecimal  v_slab_num = 0;
	CString   v_slab_dest_code = "";
	CString   v_slab_plan_dest = "";
	CString   v_pono_act = "";
	CString   v_factory_div_act = "";
	
	CString sqlstr = "";
	CString sqlstr2 = "";
	CString  sqlstr_inq = "";
	CString  sqlstr_total = "";
	CString  sqlstr_where = "";

	int     v_total_count    = 0;

	CString v_item_ename_e = "";  //各工序炉数英文名
	CString v_item_ename[100] = { "" }; //字段数组  100行
	CString v_item_ename_type[100] = { "" }; //字段类型数组  100行
	CString v_item_ename_code[100] = { "" }; //字段取值方式数组  100行

	CString v_item_ename_3[100] = { "" }; //字段数组  100行
	CString v_item_ename_p3[100] = { "" }; //字段数组  100行
	CString v_item_ename_type_3[100] = { "" }; //字段类型数组  100行
	CString v_item_ename_code_3[100] = { "" }; //字段取值方式数组  100行


	int     v_item_ename_num = 0;   //字段总数
	int     v_item_ename_num_3 = 0;//复制上炉字段总数

	CString  test = "";

	
	CString sqlstr_item_display;
	CString sqlstr_item_display_3;
	CString sqlstr_item_display_p3;

	
	int     i = 0;
	int     j = 0;

	CDbCommand cmd_inq(conn); //与DB 建立连接。
	CDbCommand cmd_sql(conn); //与DB 建立连接。
  
	CModel tpssm11("TPSSM11");
	CModel tpssm12("TPSSM12");
	CModel tpssmd1("TPSSMD1");
	CModel tmmsm21("TMMSM21");

	try
	{
		
		//获得输入参数
		//============
		if(bcls_rec->Tables[0].Columns.Contains("FACTORY_DIV"))
			v_factory_div = bcls_rec->Tables[0].Rows[0]["FACTORY_DIV"].ToString().Trim();
		if (bcls_rec->Tables[0].Columns.Contains("AREA_ID"))
			v_area_id = bcls_rec->Tables[0].Rows[0]["AREA_ID"].ToDecimal();
		if(bcls_rec->Tables[0].Columns.Contains("STATION_ID"))
			v_station_id = bcls_rec->Tables[0].Rows[0]["STATION_ID"].ToString().Trim();
		if(bcls_rec->Tables[0].Columns.Contains("STATION_NO"))
			v_station_no = bcls_rec->Tables[0].Rows[0]["STATION_NO"].ToString().Trim();

			
		 Log::Info("", __FUNCTION__, "v_factory_div  =[{0}]", v_factory_div);
		 Log::Info("", __FUNCTION__, "v_area_id      =[{0}]", v_area_id);
		 Log::Info("", __FUNCTION__, "v_station_id     =[{0}]", v_station_id	);
		 Log::Info("", __FUNCTION__, "v_station_no     =[{0}]", v_station_no	);

		

		if (v_station_id.SubstringNE(0,1) == "P")
		{
	
		}
		else if (v_station_id.SubstringNE(0,1) == "B")
		{
		
			v_item_name1 = "proc_no";
			v_item_name2 = "proc_no";
			v_table_name = "TMMSM21";//转炉
		}

		else if (v_station_id.SubstringNE(0,1) == "E")
		{
			v_item_name1 = "proc_no";
			v_item_name2 = "heat_no";
			v_table_name = "TMMSM20";//电炉
		}
		else if (v_station_id.SubstringNE(0, 1) == "M")
		{
			v_item_name1 = "proc_no";
			v_item_name2 = "heat_no";
			v_table_name = "TMMSM19";//中频炉
		}

		else if (v_station_id.SubstringNE(0,1) == "A")//吹氩
		{
			v_item_name1 = "proc_no";
			v_item_name2 = "proc_no";
			v_table_name = "TMMSM22"; //
		}
		else if (v_station_id.SubstringNE(0,1) == "R")
		{
			v_item_name1 = "proc_no";
			v_item_name2 = "proc_no";
			v_table_name = "TMMSM23"; //RH
		}
		else if (v_station_id.SubstringNE(0,1) == "L")
		{
			v_item_name1 = "proc_no";
			v_item_name2 = "proc_no";
			v_table_name = "TMMSM24";
		}
		else if (v_station_id.SubstringNE(0,1) == "V")
		{
			v_item_name1 = "proc_no";
			v_item_name2 = "proc_no";
			v_table_name = "TMMSM25";
		}
		else if (v_station_id.SubstringNE(0,1) == "C")
		{
			v_item_name1 = "heat_no";
			v_item_name2 = "heat_no";
			v_table_name = "TMMSM31";
		}
		else if (v_station_id.SubstringNE(0,1) == "I")
		{
			v_item_name1 = "heat_no";
			v_item_name2 = "heat_no";
			v_table_name = "TMMSM41";
		}

		//中频炉定制化
		if (v_station_id.SubstringNE(0, 1) == "M")
		{

			sqlstr_inq = " select t.*, t.rowid from tpssm11 t "
				" where heat_no >' ' and "
				" exists(select 1 from tpssm01  where pono = t.pono and HEAT_NO_FLAG = '1') and "
				" not exists(select 1 from tmmsm19 where pono = t.pono) "
				" order by heat_no ";
		}
		else
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）

				sqlstr_inq = " SELECT a.*, b.* "
					" FROM tpssm11 a, tpssm12 b, tpssmd1 d "
					" left join "
					+ v_table_name +
					" c on b.proc_no = c.proc_no "
					" WHERE a.factory_div = b.factory_div"
					" and a.sm_plan_no = b.sm_plan_no"
					" AND a.heat_no = b.heat_no"
					" and a.factory_div = d.factory_div"
					" AND b.dev_code = d.dev_code"
					" AND b.proc_no > ' '"
					" AND c.proc_no is null"
					" and d.station_id = @station_id "
					;

				break;

			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sqlstr_inq = " SELECT a.*, b.* "
					" FROM tpssm11 a, tpssm12 b, tpssmd1 d "
					" left join "
					+ v_table_name +
					" c on b.proc_no = c.proc_no "
					" WHERE a.factory_div = b.factory_div"
					" and a.sm_plan_no = b.sm_plan_no"
					" AND a.heat_no = b.heat_no"
					" and a.factory_div = d.factory_div"
					" AND b.dev_code = d.dev_code"
					" AND b.proc_no > ' '"
					" AND c.proc_no is null"
					" and d.station_id = @station_id ";

				break;
			}

			if (v_area_id != 0)
			{
				sqlstr_inq = sqlstr_inq + "AND b.area_id  = @area_id";
			}

			if (v_factory_div.Trim() != "" )
			{
				sqlstr_inq = sqlstr_inq + "and a.factory_div = @factory_div";
			}

			if (v_station_no != "0" && v_station_no != "")
			{
				sqlstr_inq = sqlstr_inq + "and d.station_no = @station_no";
			}
			//sqlstr_inq = sqlstr_inq + ")";

			//sqlstr_where = " AND b." + v_item_name1 + " NOT IN (SELECT " + v_item_name2 + " FROM " + v_table_name + " WHERE 1 = 1  )";

		}
		

		sqlstr = sqlstr_inq + sqlstr_where;
		Log::Info("", __FUNCTION__, "sqlstr      =[{0}]", sqlstr);
		
		cmd_sql.SetCommandText(sqlstr);
		cmd_sql.Parameters.Set("factory_div", v_factory_div);
	
		if (v_station_id == "A"){
			//吹氩站取转炉
			cmd_sql.Parameters.Set("area_id", "3");
			cmd_sql.Parameters.Set("station_id", "B");
		}
		else {
			cmd_sql.Parameters.Set("area_id", v_area_id);
			cmd_sql.Parameters.Set("station_id", v_station_id);
		}

		cmd_sql.Parameters.Set("station_no", v_station_no);
		cmd_sql.ExecuteReader(); //执行读取
		
		if(!bcls_ret->Tables[0].Columns.Contains("CODE"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING,"CODE"); 
		}
		
		if (v_area_id == 4)
		{
			if(!bcls_ret->Tables[0].Columns.Contains("REFINE_NUM"))
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING,"REFINE_NUM"); 
			}
			if(!bcls_ret->Tables[0].Columns.Contains("HEAT_NO"))
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING,"HEAT_NO"); 
			}

		}
		else if (v_area_id == 5)
		{
			if(!bcls_ret->Tables[0].Columns.Contains("PROC_NO"))
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING,"PROC_NO"); 
			}

			if (v_station_id == "C")
			{
				if (!bcls_ret->Tables[0].Columns.Contains("CAST_NO"))
				{
					bcls_ret->Tables[0].Columns.Add(DT_STRING, "CAST_NO");
				}
				if (!bcls_ret->Tables[0].Columns.Contains("CAST_DIV_NO"))
				{
					bcls_ret->Tables[0].Columns.Add(DT_STRING, "CAST_DIV_NO");
				}

			}
			
		}
		
		if(!bcls_ret->Tables[0].Columns.Contains("PONO"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING,"PONO"); 
		}
		if(!bcls_ret->Tables[0].Columns.Contains("ST_NO"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING,"ST_NO"); 
		}
		if(!bcls_ret->Tables[0].Columns.Contains("START_TIME"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING,"START_TIME"); 
		}
		if(!bcls_ret->Tables[0].Columns.Contains("END_TIME"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING,"END_TIME"); 
		}
		if(!bcls_ret->Tables[0].Columns.Contains("SM_PLAN_NO"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING,"SM_PLAN_NO"); 
		}
		if(!bcls_ret->Tables[0].Columns.Contains("DEV_CODE"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING,"DEV_CODE"); 
		}
		
		/*if(!bcls_ret->Tables[0].Columns.Contains("LADLE_NO"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING,"LADLE_NO"); 
		}*/

		
		
		if (v_area_id == 3)
		{
		
			if(!bcls_ret->Tables[0].Columns.Contains("STATION_NO"))
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING,"STATION_NO"); 
			}
			if(!bcls_ret->Tables[0].Columns.Contains("SMELT_MODE"))
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING,"SMELT_MODE"); 
			}
		}
		if (v_area_id == 4)
		{
			if(!bcls_ret->Tables[0].Columns.Contains("STATION_NO"))
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING,"STATION_NO"); 
			}
		}
		if (v_area_id == 5)
		{
			if(!bcls_ret->Tables[0].Columns.Contains("STATION_NO"))
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING,"STATION_NO"); 
			}
			if(!bcls_ret->Tables[0].Columns.Contains("SLAB_PLAN_DEST"))
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING,"SLAB_PLAN_DEST"); 
			}
			if(!bcls_ret->Tables[0].Columns.Contains("CUT_SLAB_NUM"))
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING,"CUT_SLAB_NUM"); 
			}
		}

		//ADD BY xiangping 2017/02/06 为简化实绩录入过程，增加复制上一炉功能

		//-------------------------------------------------------
		//将字段放入数组，以提高效率
		sqlstr = CString("SELECT ITEM_ENAME,ITEM_TYPE,CODE,ITEM_ENAME_P"
			"  FROM  TMMSMED54  "
			"  WHERE STATION_ID = @station_id "
			"  ORDER BY SEQ_NO ");
		
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("station_id", v_station_id);
		cmd_inq.ExecuteReader();

		while (cmd_inq.Read())
		{
			v_item_ename[i] = cmd_inq.GetString(1);
			v_item_ename_type[i] = cmd_inq.GetString(2);
			v_item_ename_code[i] = cmd_inq.GetString(3);

			Log::Info("", __FUNCTION__, "v_item_ename[i]00000  =[{0}]", v_item_ename[i]);

			Log::Info("", __FUNCTION__, "v_item_ename_type[i]000000000  =[{0}]", v_item_ename_type[i]);

	
			if (i == 0)
			{
				sqlstr_item_display = v_item_ename[i];
			}
			else
			{
				sqlstr_item_display = sqlstr_item_display + "," + v_item_ename[i]; //需显示字段
			}

			if (v_item_ename_code[i] == '3')
			{
				v_item_ename_3[j] = cmd_inq.GetString(1);
				v_item_ename_type_3[j] = cmd_inq.GetString(2);
				v_item_ename_code_3[j] = cmd_inq.GetString(3);
				v_item_ename_p3[j] = cmd_inq.GetString(4);

				

				if (j == 0)
				{
					sqlstr_item_display_3 = v_item_ename_3[j]; //上工序需显示字段
					sqlstr_item_display_p3 = v_item_ename_p3[j]; //上工序需显示字段
				}
				else
				{
					sqlstr_item_display_3 = sqlstr_item_display_3 + "," + v_item_ename_3[j];
					sqlstr_item_display_p3 = sqlstr_item_display_p3 + "," + v_item_ename_p3[j];
				}
				j++;

			}


			v_item_ename_num++;    //英文名个数
			i++;

		}
		cmd_inq.Close();

		v_item_ename_num_3 = j;

	
		Log::Info("", __FUNCTION__, "sqlstr_item_display  =[{0}]", sqlstr_item_display);

				
		for (i = 0; i < v_item_ename_num; i++)
		{
			v_item_ename_e = v_item_ename[i];

			bcls_ret->Tables[0].Columns.Add(DT_STRING, v_item_ename_e);
		}

		Log::Info("", __FUNCTION__, "sqlstr_item_display 11111 =[{0}]", sqlstr_item_display);

	
		//相关信息初始化。
		//===============
		fetchRowCount = 0;
		while(cmd_sql.Read()) //只读取单记录，可用IF 语句。
		{		
			
			cmd_sql.Fetch(tpssm11);		 //整个表结构的获取。
			cmd_sql.Fetch(tpssm12);
		
			Log::Info("", __FUNCTION__, " fetchRowCount =[{0}]",  fetchRowCount);

			Log::Info("", __FUNCTION__, " tpssm11.PONO  =[{0}]",  tpssm11["PONO"].ToString());
			Log::Info("", __FUNCTION__, " tpssm11.HEAT_NO  =[{0}]",  tpssm11["HEAT_NO"].ToString());
			Log::Info("", __FUNCTION__, " tpssm12.PROC_NO  =[{0}]", tpssm12["PROC_NO"].ToString());
			Log::Info("", __FUNCTION__, " tpssm11.ST_NO  =[{0}]",  tpssm11["ST_NO"].ToString());
			Log::Info("", __FUNCTION__, " tpssm12.DEV_CODE  =[{0}]", tpssm12["DEV_CODE"].ToString());

		
			bcls_ret->Tables[0].Rows.Add();

			if (v_factory_div == "")
			{
				f_mmsm_get_pono(tpssm11["HEAT_NO"].ToString(), v_pono_act, v_factory_div_act, conn);
			}

			Log::Info("", __FUNCTION__, "v_factory_div_act    =[{0}]", v_factory_div_act);

			sqlstr = "SELECT  STATION_NO "
					     "FROM    TPSSMD1 "
						" WHERE   FACTORY_DIV = @factory_div" 
						" AND     DEV_CODE = @dev_code";
	  
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("factory_div",v_factory_div_act); 
			cmd_inq.Parameters.Set("dev_code",tpssm12["DEV_CODE"].ToString()); 
			cmd_inq.ExecuteReader();

			if(cmd_inq.Read())
			{
				tpssmd1["STATION_NO"]	= cmd_inq.GetString(1); 
			}
			cmd_inq.Close();

			Log::Info("", __FUNCTION__, "tpssmd1.STATION_NO      =[{0}]", tpssmd1["STATION_NO"].ToString());


			if (v_area_id == 3)
			{
				bcls_ret->Tables[0].Rows[fetchRowCount]["CODE"] = tpssm11["HEAT_NO"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["STATION_NO"] = tpssmd1["STATION_NO"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["SMELT_MODE"] = tpssm11["SMELT_MODE"];
				
			}
			else if (v_area_id == 4)
			{
				bcls_ret->Tables[0].Rows[fetchRowCount]["CODE"] = tpssm12["PROC_NO"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["HEAT_NO"] = tpssm11["HEAT_NO"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["STATION_NO"] = tpssmd1["STATION_NO"];
			}
			else if (v_area_id == 5)
			{
				sqlstr = "SELECT distinct(slab_dest) "
						"FROM    TPSSM03 "
						"WHERE   PONO = @tpssm11.PONO";
						 
			  
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tpssm11.PONO",tpssm11["PONO"].ToString()); 
				cmd_inq.ExecuteReader();

				if(cmd_inq.Read())
				{
					v_slab_plan_dest = cmd_inq.GetString(1);
					
				}
				cmd_inq.Close();

				Log::Info("", __FUNCTION__, " v_slab_plan_dest =[{0}]", v_slab_plan_dest);

				v_slab_num = 0;

				f_mmsm_slab_dest(v_slab_plan_dest, v_slab_dest_code, conn);

				if (v_slab_dest_code == "HP") 
				//if (v_slab_dest.Trim() == "10")
				{

					sqlstr = "SELECT  COUNT(DISTINCT(LSLAB_NO))"
						"FROM    TPSSM03 "
						"WHERE   PONO = @tpssm11.PONO";
      

				}
				else
				{
					sqlstr = "SELECT  SUM(SLAB_NUM)"
						"FROM    TPSSM03 "
						"WHERE   PONO = @tpssm11.PONO";
				}
				  
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Clear();
				cmd_inq.Parameters.Set("tpssm11.PONO",tpssm11["PONO"].ToString()); 
				cmd_inq.ExecuteReader();

				if(cmd_inq.Read())
				{
					v_slab_num	= cmd_inq.GetDecimal(1); 
					
				}
				cmd_inq.Close();

				Log::Info("", __FUNCTION__, " v_slab_num =[{0}]", v_slab_num);

				bcls_ret->Tables[0].Rows[fetchRowCount]["CODE"] =  tpssm11["HEAT_NO"];
				if (v_station_id == "C")
				{
					bcls_ret->Tables[0].Rows[fetchRowCount]["CAST_NO"] = tpssm11["CAST_NO"];
					bcls_ret->Tables[0].Rows[fetchRowCount]["CAST_DIV_NO"] = tpssm11["CAST_DIV_NO"];
				}
				bcls_ret->Tables[0].Rows[fetchRowCount]["STATION_NO"] = tpssm11["CC_MACH_NO"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["PROC_NO"] = tpssm12["PROC_NO"];
				bcls_ret->Tables[0].Rows[fetchRowCount]["SLAB_PLAN_DEST"] = v_slab_dest;
				bcls_ret->Tables[0].Rows[fetchRowCount]["CUT_SLAB_NUM"] = v_slab_num;

			}
			
		
			bcls_ret->Tables[0].Rows[fetchRowCount]["PONO"] = tpssm11["PONO"];
			bcls_ret->Tables[0].Rows[fetchRowCount]["ST_NO"] = tpssm11["ST_NO"]; 
			bcls_ret->Tables[0].Rows[fetchRowCount]["SM_PLAN_NO"] = tpssm11["SM_PLAN_NO"];
		/*	if (tpssm11["LADLE_NO"].ToString().Trim() != "")
			{
				bcls_ret->Tables[0].Rows[fetchRowCount]["LADLE_NO"] = tpssm11["LADLE_NO"];
			}*/
			bcls_ret->Tables[0].Rows[fetchRowCount]["START_TIME"] = tpssm12["START_TIME"];
			bcls_ret->Tables[0].Rows[fetchRowCount]["END_TIME"] = tpssm12["END_TIME"];
			bcls_ret->Tables[0].Rows[fetchRowCount]["DEV_CODE"] = tpssm12["DEV_CODE"];
		
			
			if(v_area_id ==4)
			{
				//判断第几重
				sqlstr2 = " SELECT a.PROC_NO FROM TPSSM12 a"
						" WHERE heat_no= @heat_no"
						" AND area_id = @area_id"
						" AND factory_div = @factory_div "
						" AND dev_code =@dev_code"
						" ORDER BY CHARGE_NO asc";

				Log::Info("", __FUNCTION__, "sqlstr2      =[{0}]", sqlstr2);
				Log::Info("", __FUNCTION__, "tpssm11.HEAT_NO       =[{0}]", tpssm11["HEAT_NO"].ToString());
				Log::Info("", __FUNCTION__, "tpssm12.DEV_CODE  =[{0}]", tpssm12["DEV_CODE"].ToString());
				Log::Info("", __FUNCTION__, "v_factory_div    =[{0}]", v_factory_div);
				Log::Info("", __FUNCTION__, "v_area_id   =[{0}]", v_area_id);
				
				cmd_inq.SetCommandText(sqlstr2);
				cmd_inq.Parameters.Set("heat_no", tpssm11["HEAT_NO"]);
				cmd_inq.Parameters.Set("dev_code", tpssm12["DEV_CODE"].ToString());
				cmd_inq.Parameters.Set("factory_div", v_factory_div_act);
				cmd_inq.Parameters.Set("area_id", v_area_id);
				cmd_inq.ExecuteReader();
				count = 0;
				while(cmd_inq.Read())
				{
					count ++;
					Log::Trace("",__FUNCTION__,"count		= [{0}]",count);
					if(cmd_inq.GetString(1)==tpssm12["PROC_NO"].ToString())
					{
						bcls_ret->Tables[0].Rows[fetchRowCount]["REFINE_NUM"] = count;
					
						break;
					}

				}
				cmd_inq.Close(); //关闭游标
			}

			//add by xiangping 2017/02/07

			Log::Info("", __FUNCTION__, " v_table_name  =[{0}]", v_table_name);
			Log::Info("", __FUNCTION__, " v_item_name1  =[{0}]", v_item_name1);

			
			if (v_item_ename_num > 0)
			{
				sqlstr = " SELECT " + sqlstr_item_display +
					"   FROM " + v_table_name +
					"  WHERE PROC_NO = (SELECT MAX(PROC_NO) FROM  " + v_table_name + " WHERE STATION_NO = @station_no AND PROC_NO < @proc_no)";

				Log::Info("", __FUNCTION__, " sqlstr1111111111111  =[{0}]", sqlstr);


				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("proc_no", tpssm12["PROC_NO"].ToString());
				cmd_inq.Parameters.Set("station_no", tpssmd1["STATION_NO"].ToString());
				cmd_inq.ExecuteReader();

				Log::Info("", __FUNCTION__, "  tpssm12.PROC_NO  =[{0}]", tpssm12["PROC_NO"].ToString());
				Log::Info("", __FUNCTION__, " tpssmd1.STATION_NO  =[{0}]", tpssmd1["STATION_NO"].ToString());
				while (cmd_inq.Read())
				{

					for (i = 0; i < v_item_ename_num; i++)
					{
						Log::Info("", __FUNCTION__, " v_item_ename[i] =[{0}]", v_item_ename[i]);
						Log::Info("", __FUNCTION__, " v_item_ename_code[i] =[{0}]", v_item_ename_code[i]);

						if (bcls_ret->Tables[0].Columns.Contains(v_item_ename[i]))
						{
							if (v_item_ename_type[i] == 'C')
							{
								if (v_item_ename_code[i] != '3')
								{
									bcls_ret->Tables[0].Rows[fetchRowCount][v_item_ename[i]] = cmd_inq.GetString(i + 1);
								}

							}
							else
							{
								if (v_item_ename_code[i] == '2')//需要加1
								{
									bcls_ret->Tables[0].Rows[fetchRowCount][v_item_ename[i]] = cmd_inq.GetDecimal(i + 1) + 1;
								}
								else if (v_item_ename_code[i] == '1')
								{
									bcls_ret->Tables[0].Rows[fetchRowCount][v_item_ename[i]] = cmd_inq.GetDecimal(i + 1);
								}

							}

						}
					}

				}
				cmd_inq.Close();

				//针对上工序

				//先找到该工序的上工序是什么
				if (v_area_id > 3)
				{
		
					sqlstr = " SELECT SUBSTR(DEV_CODE, 1, 1)"
						" FROM TPSSM12 WHERE HEAT_NO = @heat_no"
						" AND CHARGE_NO = (SELECT CHARGE_NO FROM TPSSM12 WHERE HEAT_NO = @heat_no and PROC_NO = @proc_no) - 1 ";

					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Clear();
					cmd_inq.Parameters.Set("heat_no", tpssm11["HEAT_NO"].ToString());
					cmd_inq.Parameters.Set("proc_no", tpssm12["PROC_NO"].ToString());
					cmd_inq.ExecuteReader();

					if (cmd_inq.Read())
					{
						v_station_id_up = cmd_inq.GetString(1);
					}
					cmd_inq.Close();


					Log::Info("", __FUNCTION__, " v_station_id_up  =[{0}]", v_station_id_up);

					Log::Info("", __FUNCTION__, " sqlstr_item_display_3  =[{0}]", sqlstr_item_display_3);

					if (v_station_id_up.Trim() == "B")
					{
						v_table_name_3 = "TMMSM21";
						sqlstr_item_display_3 = sqlstr_item_display_p3;
					}
					else if (v_station_id_up.Trim() == "E")
					{
						v_table_name_3 = "TMMSM20";
					}
					else if (v_station_id_up.Trim() == "R")
					{
						v_table_name_3 = "TMMSM23";
					}
					else if (v_station_id_up.Trim() == "L")
					{
						v_table_name_3 = "TMMSM24";
					}
					else if (v_station_id_up.Trim() == "V")
					{
						v_table_name_3 = "TMMSM25";
					}

					Log::Info("", __FUNCTION__, " v_table_name_3  =[{0}]", v_table_name_3);


					sqlstr = " SELECT " + sqlstr_item_display_3 +
						"   FROM " + v_table_name_3 +
						"  WHERE HEAT_NO = @heat_no";



					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("heat_no", tpssm11["HEAT_NO"].ToString());
					cmd_inq.ExecuteReader();

					Log::Info("", __FUNCTION__, "  tpssm11.HEAT_NO  =[{0}]", tpssm11["HEAT_NO"].ToString());
					Log::Info("", __FUNCTION__, "  v_item_ename_num_3  =[{0}]", v_item_ename_num_3);

					while (cmd_inq.Read())
					{
						for (j = 0; j < v_item_ename_num_3; j++)
						{

							Log::Info("", __FUNCTION__, " v_item_ename_code_3[j] =[{0}]", v_item_ename_code_3[j]);

							if (v_item_ename_code_3[j] == '3')
							{
								if (v_item_ename_type_3[j] == 'C')
								{
									Log::Info("", __FUNCTION__, " v_item_ename_code_3[j] =[{0}]", v_item_ename_code_3[j]);
									bcls_ret->Tables[0].Rows[fetchRowCount][v_item_ename_3[j]] = cmd_inq.GetString(j + 1);
								}
								else
								{
									bcls_ret->Tables[0].Rows[fetchRowCount][v_item_ename_3[j]] = cmd_inq.GetDecimal(j + 1);
								}

							}
							Log::Info("", __FUNCTION__, " [{0}] = [{1}]", CString(v_item_ename_3[j]), CString(bcls_ret->Tables[0].Rows[fetchRowCount][v_item_ename_3[j]]));
						}

					}
					cmd_inq.Close();

				}
			}

									
			fetchRowCount++;
	
		}

		

		if(fetchRowCount==0)
		{
			bcls_ret->Tables[0].Rows.Add();
		}

		cmd_sql.Close(); //关闭游标



	 }
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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

