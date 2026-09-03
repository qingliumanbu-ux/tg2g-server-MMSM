/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-07-04
Description: 获取材料号
remark:本函数目前暂不考虑按批管理
***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
/***** C++ 的业务头文件部分 *****/ 



//外部函数声明
BM2_FUNCTION_EXPORT
 int f_mmsm_matno_catch_sm(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
		/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm_matno_catch";                //定义函数英文名称  
	CString FunctionCname = "获取材料号";              //定义函数中文名称
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
 
	/****** 自定义变量 ***** */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;
	int random_len = 0;
	int cut_down_id = 0;
	CString c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");          //当前时间
	CString v_func_id = "";
	CString v_mat_no = "";
	CString v_heat_no = "";
	CString v_pono = "";
	CString v_slab_type = "";
	CString v_code_class = "";
	CString c_mat_no = "";
	CString cut_str = "";
	CDecimal n_count = 0;             //校验是否已存在该记录
	CDecimal v_mat_id_max = 0;        //记录本次调用中，传入记录中的锭坯序列号最大值
	int i_cut_len = 0;
	CString c_mat_id_max = "";
	CDecimal i_mat_id_max = 0;
	CString cut_seqid = "";
	CDecimal v_seq_max = 0;
	CString c_seq_max = " ";

	int fetchRowCount = 0;
	CString sqlstr = "";
	EIClass inBlock;          //材料主档信息处理用
	//EIClass outBlock;
	CString trace_flag = "Y";    //N- 不显示履历  Y-反之
	CModel ted54("TED54");
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");

	CDbCommand cmd_inq(conn);

	try
	{
		bcls_ret->Tables[0].Clear();
		bcls_ret->Tables[0].Copy(bcls_rec->Tables[0]);
		
		if (bcls_rec->Tables[0].Columns.Contains("TRACE_FLAG"))
		{
			trace_flag = bcls_rec->Tables[0].Rows[0]["TRACE_FLAG"].ToString();
		}
		//Log::Trace("", __FUNCTION__, "履历开关：[{0}]", trace_flag);
		if (!bcls_ret->Tables[0].Columns.Contains("MAT_NO"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAT_NO");//实物材料号
		}
		if (!bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "MAT_NO");//实物材料号
		}

		if (!bcls_ret->Tables[0].Columns.Contains("MAT_CUT_SEQID"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAT_CUT_SEQID");//实物材料序号
		}
		if (!bcls_rec->Tables[0].Columns.Contains("MAT_CUT_SEQID"))
		{
			bcls_rec->Tables[0].Columns.Add(DT_STRING, "MAT_CUT_SEQID");//实物材料序号
		}

		if (!bcls_ret->Tables[0].Columns.Contains("MAT_TUBE"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "MAT_TUBE");//材料根数
		}
		if (!bcls_ret->Tables[0].Columns.Contains("FIX_SLAB_NUM"))
		{
			bcls_ret->Tables[0].Columns.Add(DT_STRING, "FIX_SLAB_NUM");//定尺板坯块数
		}
		
		if (!bcls_rec->Tables[0].Columns.Contains("FUNC_ID"))
		{
			//不传配置代码，以SLAB_TYPE来判断
			if (!bcls_rec->Tables[0].Columns.Contains("SLAB_TYPE"))
			{
				sprintf(s.msg, "传入参数FUNC_ID与SLAB_TYPE都不存在。");
				//strcpy(s.sysmsg,s.msg);
				throw CApplicationException(-1, s.msg, log.Location);

			}
			v_slab_type = bcls_rec->Tables[0].Rows[0]["SLAB_TYPE"].ToString();
			sqlstr = "select CODE,CODE_DESC_2_CONTENT,CODE_DESC_4_CONTENT from tep0002 "
					 " where CODE_CLASS = 'M00M'"
				     " AND CODE_DESC_2_CONTENT = @v_slab_type "
					 " AND CODE_DESC_4_CONTENT = '1' ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("v_slab_type", v_slab_type);
			cmd_inq.ExecuteReader();

			Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
			if (cmd_inq.Read())
			{
				ted54["FUNC_ID"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
			//Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
			if (trace_flag.Trim() == "Y")
			{
				Log::Trace("", __FUNCTION__, "通过铸坯类型[{0}]获取配置代码[{1}]", v_slab_type, ted54["FUNC_ID"].ToString());
			}
		}
		else
		{
			ted54["FUNC_ID"] = bcls_rec->Tables[0].Rows[0]["FUNC_ID"].ToString();
			if (trace_flag.Trim() == "Y")
			{
				Log::Trace("", __FUNCTION__, "获取配置代码[{0}]", ted54["FUNC_ID"].ToString());
			}
			
		}
		if (ted54["FUNC_ID"].ToString().Trim() == "")
		{
			sqlstr = "select CODE,CODE_DESC_2_CONTENT,CODE_DESC_4_CONTENT from tep0002 "
				" where CODE_CLASS = 'M00M'"
				" AND CODE_DESC_3_CONTENT = '1' ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("v_slab_type", v_slab_type);
			cmd_inq.ExecuteReader();

			Log::Trace("", __FUNCTION__, "sqlstr[{0}]", sqlstr);
			if (cmd_inq.Read())
			{
				ted54["FUNC_ID"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
			if (trace_flag.Trim() == "Y")
			{
				Log::Trace("", __FUNCTION__, "通过配置顺序获取配置代码[{1}]", ted54["FUNC_ID"].ToString());
			}
			if (ted54["FUNC_ID"].ToString().Trim() == "")
			{
				sprintf(s.msg, "未获取参数FUNC_ID，材料计算失败。");
				//strcpy(s.sysmsg,s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
		}

		Log::Trace("", __FUNCTION__, "材料号规则号[{0}]", ted54["FUNC_ID"].ToString());
		int count = ted54.QueryCount("FUNC_ID");
		Log::Trace("", __FUNCTION__, "材料号规则号[{0}]配置项个数[{1}]", ted54["FUNC_ID"].ToString(), count);
		if (trace_flag.Trim() == "Y")
		{
			Log::Trace("", __FUNCTION__, "材料号规则号[{0}]配置项个数[{1}]", ted54["FUNC_ID"].ToString(), count);
			//Log::Trace("", __FUNCTION__, "需计算材料号[{0}]个", bcls_rec->Tables[0].Rows.get_Count());
		}

		/* 获取输入参数*/

		Log::Trace("", __FUNCTION__, "[{0}]次处理", bcls_rec->Tables[0].Rows.get_Count());

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			if (trace_flag.Trim() == "Y")Log::Trace("", __FUNCTION__, "第[{0}]次处理", i);
			
			//判断传入数据中是否存在最大序号
			random_len = 0;
			if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			{
				c_mat_no = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString();
				v_mat_no = bcls_rec->Tables[0].Rows[i]["MAT_NO"].ToString();
				if (trace_flag.Trim() == "Y")Log::Trace("", __FUNCTION__, "外部传入材料[{0}]", c_mat_no);
				if (c_mat_no.Trim()>"")
				{
					if (trace_flag.Trim() == "Y")Log::Trace("", __FUNCTION__, "材料不为空，记录最大材料号");
					cut_down_id = 0;
					for (int n = 1; n <= count; n++)
					{
						ted54["SEQ_NO"] = n;
						ted54.Query("FUNC_ID,SEQ_NO");
						int SEQ_ID = atoi((const char*)ted54["ITEM_LEN"].ToString());
						
						if (ted54["ITEM_MUST_FLAG"].ToString() == "1" && ted54["FORM_EDIT_FLAG"].ToString() != "1")
						{
							//必需项目标记做为继承ITEM 舍去
							if (ted54["ALIGNMENT"].ToString().Trim() != "2")
							{
								c_mat_no = c_mat_no.SubstringNE(SEQ_ID);
							}
							else
							{
								//对齐方式居中，表示此字段不定长，以传入长度为准
								random_len = 1;
								;
							}
						
						}
						else if (ted54["FORM_EDIT_FLAG"].ToString().Trim() == "1")
						{
							//画面可编辑标记作为自增序列 记录其序列长度
							if (ted54["ALIGNMENT"].ToString().Trim() != "2")
							{
								cut_down_id += SEQ_ID;
								v_code_class = ted54["CODE_CLASS"];

								if (random_len != 0 && n != count)//前方有不定长字段且序号不是最后一项，无法截取出序号。清空材料号重新计算
								{
									bcls_rec->Tables[0].Rows[i]["MAT_NO"] = " ";
									i--;
									continue;
								}
								
							}
							else
							{
								//对齐方式居中，表示此字段不定长，序号字段不定长，无法判断序号值，清空材料号重新计算
								bcls_rec->Tables[0].Rows[i]["MAT_NO"] = " ";
								i--;
								continue;
								;
							}
							
						}
						else
						{
							continue;
						}
					}
					//Log::Trace("", __FUNCTION__, "本次序号=[{0}]", cut_down_id);
					if (random_len != 0)
					{
						c_mat_id_max = c_mat_no.Substring(0, cut_down_id);//定长：截位即可
					}
					else
					{
						c_mat_id_max = c_mat_no.Substring(c_mat_no.GetLength() - cut_down_id, cut_down_id);//不定长且序号在最后一项(序号定长)：倒序截位
					}
					Log::Info("", __FUNCTION__, "传入数据材料最大序号[{0}]", c_mat_id_max);
					//若配置中有代码，表示该序列号存在其他编码规则，非简单序号自增（有字母或其他编码） 获取其数字型序号
					if (v_code_class.Trim() > "")
					{
						sqlstr = "select CODE,to_number(CODE_DESC_1_CONTENT) from tep0002 "
							" where CODE_CLASS = @v_code_class AND CODE = @c_mat_id_max ";
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("v_code_class", v_code_class);
						cmd_inq.Parameters.Set("c_mat_id_max", c_mat_id_max);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							i_mat_id_max = cmd_inq.GetDecimal(2);
						}
						cmd_inq.Close();
					}
					else
					{
						i_mat_id_max = atoi((const char*)c_mat_id_max);
					}
					

					if (i_mat_id_max > v_mat_id_max)
					{
						v_mat_id_max = i_mat_id_max;
					}

					if (trace_flag.Trim() == "Y")Log::Trace("", __FUNCTION__, "传入材料号[{0}]序号=[{1}]", v_mat_no, v_mat_id_max);
					bcls_ret->Tables[0].Rows[i]["MAT_CUT_SEQID"] = v_mat_id_max;
					bcls_rec->Tables[0].Rows[i]["MAT_CUT_SEQID"] = v_mat_id_max;

					
					
					//若该材料号已使用，清空材料号后直接返回循环
					tmmsm01["MAT_NO"] = v_mat_no;
					hmmsm01["MAT_NO"] = v_mat_no;
					if ((tmmsm01.QueryCount("MAT_NO") > 0 || hmmsm01.QueryCount("MAT_NO") > 0) && ted54["ITEM_KEY_FLAG"].ToString() == "1")
					{
						bcls_rec->Tables[0].Rows[i]["MAT_NO"] = " ";
						i--;
						continue;
					}
					continue;
				}
			}
			//根据TED54配置 生成材料号
			for (int n = 1; n <= count; n++)
			{
				v_mat_no = v_mat_no.Trim();
			
				ted54["SEQ_NO"] = n;
				ted54.Query("FUNC_ID,SEQ_NO");
				if (trace_flag.Trim() == "Y")
				{
					Log::Trace("", __FUNCTION__, "第[{0}]次调用", i);
					Log::Trace("", __FUNCTION__, "第[{0}]个配置项目", n);
					Log::Trace("", __FUNCTION__, "ITEM_ENAME=[{0}]", ted54["ITEM_ENAME"].ToString());
				}
				if (ted54["ALIGNMENT"].ToString().Trim() != "2")
				{
					if (ted54["ITEM_DEFAULT_VALUE"].ToString().Trim() == "")ted54["ITEM_DEFAULT_VALUE"] = "0";
				}
				
				if (ted54["ITEM_MUST_FLAG"].ToString() == "1")
				{
					if (bcls_rec->Tables[0].Columns.Contains(ted54["ITEM_ENAME"].ToString()))
					{
						cut_str = bcls_rec->Tables[0].Rows[i][ted54["ITEM_ENAME"].ToString()].ToString();
						if (trace_flag.Trim() == "Y")Log::Trace("", __FUNCTION__, "cut_str1=[{0}],ted54.ITEM_LEN =[{1}]", cut_str, ted54["ITEM_LEN"].ToString());
						if (ted54["ALIGNMENT"].ToString().Trim() != "2")
						{
							i_cut_len = atoi((const char*)ted54["ITEM_LEN"].ToString()) > cut_str.GetLength() ? cut_str.GetLength() : atoi((const char*)ted54["ITEM_LEN"].ToString());
						}
						else
						{
							i_cut_len = cut_str.GetLength();
						}
						cut_str = cut_str.Substring(0, i_cut_len);
						Log::Trace("", __FUNCTION__, "c_111111111=[{0}]", cut_str);
						for (int x = i_cut_len; x < atoi((const char*)ted54["ITEM_LEN"].ToString()); x++)
						{
							if (cut_str.Trim().GetLength() < atoi((const char*)ted54["ITEM_LEN"].ToString()))
							{
								if (ted54["ALIGNMENT"].ToString().Trim()!="1")
								{
									cut_str = ted54["ITEM_DEFAULT_VALUE"].ToString().Trim() + cut_str;
								}
								else
								{
									cut_str = cut_str + ted54["ITEM_DEFAULT_VALUE"].ToString().Trim();
								}
								
							}
						}
						
						v_mat_no += cut_str;
					}
					else
					{
						sprintf(s.msg, "传入参数[{0}]不存在。", (const char*)ted54["ITEM_ENAME"].ToString());
						//strcpy(s.sysmsg,s.msg);
						throw CApplicationException(-1, s.msg, log.Location);
					}
					
					if (bcls_rec->Tables[0].Rows[0]["SLAB_TYPE"].ToString() == "3")//三明方坯定制炉号加流号                                                             
					{
						v_mat_no = v_mat_no + bcls_rec->Tables[0].Rows[0]["STRAND_NO"].ToString();
					}
					Log::Trace("", __FUNCTION__, "SLAB_TYPE[{0}]STRAND_NO[{1}]", bcls_rec->Tables[0].Rows[0]["SLAB_TYPE"].ToString(),
						bcls_rec->Tables[0].Rows[0]["STRAND_NO"].ToString());
					Log::Trace("", __FUNCTION__, "c_mat_no1=[{0}]", v_mat_no);
				}
				else
				{
					if (bcls_rec->Tables[0].Rows[0]["SLAB_TYPE"].ToString() == "3")//三明方坯走这段拼接逻辑  
					{
						CDecimal mat_seq = v_mat_no.Trim().GetLength() + 1;
						CDecimal mat_seq1 = atoi((const char*)ted54["ITEM_LEN"].ToString());;
						CDecimal v_seq_max = 0;
						CString c_seq_max = " ";
						cut_str = "";
						//Log::Trace("", __FUNCTION__, "mat_seq=[{0}];mat_seq1[{1}]", mat_seq, mat_seq1);
						if (ted54["FORM_EDIT_FLAG"].ToString().Trim() == "1")
						{
							v_code_class = ted54["CODE_CLASS"];
							c_mat_no = v_mat_no + "%";
							if (trace_flag.Trim() == "Y")Log::Trace("", __FUNCTION__, "c_mat_no=[{0}]", c_mat_no);
							sqlstr = ted54["ITEM_DATASOURCE"];
							sqlstr += ted54["REGULAR_EXPRESS"].ToString();
							//Log::Trace("", __FUNCTION__, "REGULAR_EXPRESS=[{0}]", ted54["REGULAR_EXPRESS"].ToString());
							if (bcls_rec->Tables[0].Columns.Contains("PONO"))
							{
								v_pono = bcls_rec->Tables[0].Rows[i]["PONO"].ToString();
								sqlstr += " and PONO = @v_pono   ";
								if (trace_flag.Trim() == "Y"); Log::Trace("", __FUNCTION__, "v_pono=[{0}]", v_pono);
							}
							if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
							{
								v_heat_no = bcls_rec->Tables[0].Rows[i]["HEAT_NO"].ToString();
								sqlstr += " and HEAT_NO = @v_heat_no ";
								Log::Trace("", __FUNCTION__, "v_heat_no=[{0}]", v_heat_no);
							}

							if (trace_flag.Trim() == "Y"); Log::Trace("", __FUNCTION__, "sqlstr=[{0}]", sqlstr);
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.Parameters.Set("mat_no", c_mat_no);
							cmd_inq.Parameters.Set("v_pono", v_pono);
							cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
							cmd_inq.ExecuteReader();
							if (cmd_inq.Read())
							{
								c_seq_max = cmd_inq.GetString(1);
								Log::Trace("", __FUNCTION__, "c_seq_max a=[{0}]", c_seq_max);
							}
							cmd_inq.Close();
							if (trace_flag.Trim() == "Y")Log::Trace("", __FUNCTION__, "c_seq_max aa=[{0}]", c_seq_max);
							if (v_code_class.Trim() > "")
							{
								sqlstr = "select CODE,to_number(CODE_DESC_1_CONTENT) from tep0002 "//MM0M
									" where CODE_CLASS =@ted54.CODE_CLASS AND CODE = @c_seq_max ";
								cmd_inq.SetCommandText(sqlstr);
								cmd_inq.Parameters.Set("ted54.CODE_CLASS", ted54["CODE_CLASS"].ToString());
								cmd_inq.Parameters.Set("c_seq_max", c_seq_max);
								cmd_inq.ExecuteReader();
								if (cmd_inq.Read())
								{
									v_seq_max = cmd_inq.GetDecimal(2);
								}
								cmd_inq.Close();
								if (trace_flag.Trim() == "Y"); Log::Trace("", __FUNCTION__, "v_code_class aa=[{0}]", v_code_class);
							}
							else
							{
								v_seq_max = atoi((const char*)c_seq_max);
							}
							v_seq_max = v_seq_max + 1;
							Log::Trace("", __FUNCTION__, "v_seq_max=[{0}]", v_seq_max);
							CDecimal id = 0;
							for (id = v_seq_max; id <= v_mat_id_max; )
							{
								Log::Trace("", __FUNCTION__, "传入板坯序号【{0}】存在比当前序号[{1}]大，序号自增", v_seq_max, v_mat_id_max);
								if (v_seq_max <= v_mat_id_max)
								{
									v_seq_max = v_seq_max + 1;
								}
								id = id + 1;
							}
							if (v_code_class.Trim() > "")
							{
								sqlstr = "select CODE,to_number(CODE_DESC_1_CONTENT) from tep0002 "
									" where CODE_CLASS =@ted54.CODE_CLASS AND CODE_DESC_1_CONTENT = @v_seq_max ";
								cmd_inq.SetCommandText(sqlstr);
								cmd_inq.Parameters.Set("v_seq_max", v_seq_max.ToString());
								cmd_inq.Parameters.Set("ted54.CODE_CLASS", ted54["CODE_CLASS"].ToString().Trim());
								cmd_inq.ExecuteReader();
								if (cmd_inq.Read())
								{
									c_seq_max = cmd_inq.GetString(1);
								}
								cmd_inq.Close();
							}
							else
							{
								c_seq_max = v_seq_max.ToString();
							}
							v_mat_id_max = v_seq_max;
							Log::Trace("", __FUNCTION__, "c_seq_max=[{0}]", c_seq_max);
							cut_str = c_seq_max;
							bcls_ret->Tables[0].Rows[i]["MAT_CUT_SEQID"] = c_seq_max;
							bcls_rec->Tables[0].Rows[i]["MAT_CUT_SEQID"] = c_seq_max;
						}
						else
						{
							//c_seq_max = "0";
							if (ted54["ITEM_DATASOURCE"].ToString().Trim() != "")
							{
								sqlstr = ted54["ITEM_DATASOURCE"];
								cmd_inq.SetCommandText(sqlstr);
								cmd_inq.ExecuteReader();
								if (cmd_inq.Read())
								{
									cut_str = cmd_inq.GetString(1);
								}
								cmd_inq.Close();
							}
							else
							{
								if (ted54["ALIGNMENT"].ToString().Trim() != "1")
								{
									cut_str = ted54["ITEM_DEFAULT_VALUE"].ToString().Trim() + cut_str;
								}
								else
								{
									cut_str = cut_str + ted54["ITEM_DEFAULT_VALUE"].ToString().Trim();
								}
							}

						}
						for (int x = 0; x < mat_seq1; x++)
						{
							if (cut_str.Trim().GetLength() < mat_seq1)
							{
								if (ted54["ALIGNMENT"].ToString().Trim() != "1")
								{
									cut_str = ted54["ITEM_DEFAULT_VALUE"].ToString().Trim() + cut_str;
								}
								else
								{
									cut_str = cut_str + ted54["ITEM_DEFAULT_VALUE"].ToString().Trim();
								}
							}
						}
						v_mat_no += cut_str;
						if (trace_flag.Trim() == "Y")Log::Trace("", __FUNCTION__, "STEP [{0}]v_mat_no=[{1}]", n, v_mat_no);


						continue;
					}
					else if (bcls_rec->Tables[0].Rows[0]["SLAB_TYPE"].ToString() == "1")
					{
		
						/*if (v_seq_max == 0)
						{
							sqlstr = "select nvl( max(SUBSTR(MAT_NO,9,1)),'0') as seq  from tmmsm33 WHERE  MAT_NO LIKE @mat_no||'%' ";
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.Parameters.Set("mat_no", v_mat_no);
							cmd_inq.ExecuteReader();
							if (cmd_inq.Read())
							{
								c_seq_max = cmd_inq.GetString(1);
								Log::Trace("", __FUNCTION__, "c_seq_max 111=[{0}]", c_seq_max);
							}
							cmd_inq.Close();
			
						}
					
						
						sqlstr = "SELECT CODE_DESC_1_CONTENT FROM TEP0002 WHERE  CODE_CLASS ='MMCC' AND CODE =@c_seq_max ";
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.Parameters.Set("c_seq_max", c_seq_max);
						cmd_inq.ExecuteReader();
						if (cmd_inq.Read())
						{
							c_seq_max = cmd_inq.GetString(1);
							Log::Trace("", __FUNCTION__, "c_seq_max 222=[{0}]", c_seq_max);

						}
						cmd_inq.Close();
						
						

						v_seq_max = atoi((const char*)c_seq_max);
						v_seq_max = v_seq_max + 1;

						if (v_seq_max >= 10)
						{
							sqlstr = "SELECT CODE FROM TEP0002 WHERE  CODE_CLASS ='MMCC' AND CODE_DESC_1_CONTENT =@v_seq_max ";
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.Parameters.Set("v_seq_max", v_seq_max.ToString());
							cmd_inq.ExecuteReader();
							if (cmd_inq.Read())
							{
								c_seq_max = cmd_inq.GetString(1);
								Log::Trace("", __FUNCTION__, "c_seq_max 333=[{0}]", c_seq_max);

							}
							cmd_inq.Close();
						}
						else
						{
							c_seq_max = v_seq_max.ToString();
						}
						Log::Trace("", __FUNCTION__, "c_seq_max 444=[{0}]", c_seq_max);
						v_mat_no = v_mat_no + c_seq_max + "0";

						Log::Trace("", __FUNCTION__, "c_mat_no2=[{0}]", v_mat_no);*/
				
						if (v_seq_max == 0)
						{
							sqlstr = "select nvl( max(SUBSTR(MAT_NO,10,1)),'0') as seq  from tmmsm33 WHERE  MAT_NO LIKE @mat_no||'%' ";
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.Parameters.Set("mat_no", v_mat_no);
							cmd_inq.ExecuteReader();
							if (cmd_inq.Read())
							{
								c_seq_max = cmd_inq.GetString(1);
								Log::Trace("", __FUNCTION__, "c_seq_max 000=[{0}]", c_seq_max);
							}
							cmd_inq.Close();

						}

						if (c_seq_max >= "9")
						{
							if (c_seq_max == "9")c_seq_max = "@";// ASCII码里字符A代表的十进制数字65前一位64所对应的就是字符@
							sqlstr = "SELECT  chr(to_number(ABS(ASCII(@seq) - 55))+55+1)   FROM SYSIBM.SYSDUMMY1 ";
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.Parameters.Set("seq", c_seq_max);
							cmd_inq.ExecuteReader();
							if (cmd_inq.Read())
							{
								c_seq_max = cmd_inq.GetString(1);
								Log::Trace("", __FUNCTION__, "c_seq_max 999=[{0}]", c_seq_max);
							}
							cmd_inq.Close();

							v_seq_max = v_seq_max+1;
						}
						else
						{
							v_seq_max = atoi((const char*)c_seq_max);
							v_seq_max = v_seq_max + 1;
							c_seq_max = v_seq_max.ToString();
						}

						Log::Trace("", __FUNCTION__, "c_seq_max qqq=[{0}]", c_seq_max);
						v_mat_no = v_mat_no + c_seq_max + "0";
						Log::Trace("", __FUNCTION__, "c_mat_no zzz=[{0}]", v_mat_no);
                    }
					
				}

			}
			////若该材料号已使用，清空材料号后直接返回循环
			//tmmsm01["MAT_NO"] = v_mat_no;
			//hmmsm01["MAT_NO"] = v_mat_no;
			//if ((tmmsm01.QueryCount("MAT_NO") > 0 || hmmsm01.QueryCount("MAT_NO") > 0) && ted54["ITEM_KEY_FLAG"].ToString() == "1")
			//{
			//	bcls_rec->Tables[0].Rows[i]["MAT_NO"] = " ";
			//	i--;
			//v_mat_id_max = v_mat_id_max+1;
			//	continue;
			//}


			bcls_ret->Tables[0].Rows[i]["MAT_NO"] = v_mat_no;
			cut_seqid = bcls_ret->Tables[0].Rows[i]["MAT_CUT_SEQID"].ToString();
			if (trace_flag.Trim() == "Y")Log::Trace("", __FUNCTION__, "row_id[{0}]get_mat_no=[{1}] cut_seqid[{2}]", i, v_mat_no, cut_seqid);
			bcls_rec->Tables[0].Rows[i]["MAT_NO"] = v_mat_no;

		}
		
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
	if (doFlag < 0)
	{
		//Log::Trace("", __FUNCTION__, "******************输出传入数据开始**********************");
		//tmmsm33.Print();
		//Log::Trace("", __FUNCTION__, "******************输出传入数据结束**********************");
	}
	return doFlag;

}
