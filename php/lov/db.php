<? #-*-c-*-
# $Header$
#
# Generic database wrapper.
# This shall be XHTML 1.0 compliant.
#
# The database has a "slow start",
# such that you still can access pages which do not use the database
# if the database is down.
#
# This is GNU GPL v2 or higher.
#
# $Log$
# Revision 1.1  2008-06-22 11:32:20  tino
# First checkin
#

$db=null;

class Db
{
  var	$db, $type, $name;

  function _start()
    {
      $this->db	= @$this->_open($this->name);
      if (!$this->db)
	$this->oops("cannot open ".$this->type." DB ".$this->name);
      $this->_init();
    }

  function Db($name)
    {
      $this->name=$name;
    }


  function oops($s)
    {
      die("OOPS ".htmlentities($s));
    }
  function debug_escape($s)
    {
      echo str_replace(array("--","\$","\""),array("\\-\\-","\\\$", "\\\""), str_replace("\\","\\\\",$s));
    }
  function debug()
    {
      $a	= func_get_args();
      if (count($a)==1)
	$a	= $a[0];
      if (is_string($a) && substr($a,0,6)=="select")
	{
	  echo "<!--\n";
	  echo "sqlite ";
	  echo $this->name;
	  echo " \"";
	  $this->debug_escape($a);
	  echo "\"\n#-->";
	  return;
	}
      echo "<!-- ";
      $this->debug_escape(str_replace("   ", "", str_replace("\n"," ",print_r($a, true))));
      echo " -->";
    }

  function _prep($q,$a)
    {
      if (!preg_match("!^[A-Za-z0-9][-,='_A-Za-z0-9 +*/?()]*$!", $q))
	$this->oops("query $q");

      $p	= explode("?", $q);
      $s	= $p[0];
      if (is_array($a))
	{
	  reset($a);
	  for ($i=1; $i<count($p); $i++)
	    {
	      $v	= $this->_escape($a[$i-1]);
	      if ((substr($s,-1)!="'" || substr($p[$i],0,1)!="'") && !preg_match("/^[0-9][0-9]*$/", $v))
		$this->oops("arg $i in query $q: $v");
	      $s	.= $v;
	      $s	.= $p[$i];
	    }
	}
      $this->debug($s);
      return $s;
    }

  function _rowarray($r,$f)
    {
      $a	= array();
      while (($d=$this->$f($r))!==false)
	$a[]	= $d;
      return $a;
    }

  # Run Query
  function _q($s,$a=0)
    {
      if (!$this->db)
        $this->_start();
      $r	= $this->_query($this->_prep($s,$a));
      $this->debug($r);
      return $r;
    }
  function _qq($s,$a=0)
    {
      $r	= $this->_q($s,$a);
      $this->debug($r);
      if (!$r)
	$this->oops("no rows for $q");
      return $r;
    }

  # Close Query
  function _c($r) { }

  # Run a query without result
  function q0($q,$a=0)
    {
      $r	= $this->_q($q,$a);
      if ($r===false)
	return false;
      $this->_c($r);
      return true;
    }
  function qok($q,$a=0)
    {
      $r	= $this->_qq($q,$a);
      $this->_c($r);
    }

  # Return exactly one singleton argument
  function q1($q,$a=0)
    {
      $r	= $this->_q($q,$a);
      if (!$r)
	return "";
      $v	= $this->_single($r);
      $this->_c($r);
      $this->debug("q1", $v);
      return $v;
    }
  # for all rows
  function q1all($q,$a=0)
    {
      $r	= $this->_q($q,$a);
      if (!$r)
	return false;
      $a	= $this->_rowarray($r, "_single");
      $this->_c($r);
      $this->debug("q1all", $a);
      return $a;
    }
  # value must exist
  function q1arr($q,$a=0)
    {
      $r	= $this->_qq($q,$a);
      $a	= $this->_rowarray($r, "_single");
      $this->_c($r);
      $this->debug("q1arr", $a);
      return $a;
    }
  # One complete row as array
  function qrow($q,$a=0)
    {
      $r	= $this->_q($q,$a);
      if (!$r)
	return false;
      $a	= $this->_row($r);
      $this->_c($r);
      $this->debug("qrow", $a);
      return $a;
    }
  # All rows as array of arrays
  function qall($q,$a=0)
    {
      $r	= $this->_q($q,$a);
      if (!$r)
	return false;
      $a	= $this->_all($r);
      $this->_c($r);
      $this->debug("qall", $a);
      return $a;
    }
  # value must exist
  function qarr($q,$a=0)
    {
      $r	= $this->_q($q,$a);
      if (!$r)
	$this->oops("no rows for $q");
      $a	= $this->_all($r);
      $this->_c($r);
      $this->debug("qarr", $a);
      return $a;
    }

  # Query assoc

  # tuple as array
  function q2all($q,$a=0)
    {
      $a	= $this->qall($q,$a);
      if (!$a)
	return false;
      $b	= array();
      foreach ($a as $v)
	$b[$v[0]]=$v[1];
      $this->debug("q2all", $b);
      return $b;
    }
  # value must exist
  function q2arr($q,$a=0)
    {
      $a	= $this->qarr($q,$a);
      $b	= array();
      foreach ($a as $v)
	$b[$v[0]]=$v[1];
      $this->debug("q2arr", $b);
      return $b;
    }

  # vector with first element is the array key
  function qxall($q,$a=0)
    {
      $a	= $this->qall($q,$a);
      if (!$a)
	return false;
      $b	= array();
      foreach ($a as $v)
	$b[$v[0]]=array_slice($a,1);
      $this->debug("qxall", $b);
      return $b;
    }
  # value must exist
  function qxarr($q,$a=0)
    {
      $a	= $this->qarr($q,$a);
      $b	= array();
      foreach ($a as $v)
	$b[$v[0]]=array_slice($a,1);
      $this->debug("qxarr", $b);
      return $b;
    }

  # transactional
  function begin()
    {
      $this->qok("begin");
    }
  function rollback()
    {
      $this->qok("rollback");
    }
  function end()
    {
      $this->qok("commit");
    }

  # next increment of a column
  function inc($table,$column)
    {
      return $this->q1("select 1+ifnull(max($column),0) from $table");
    }
};

# Convenience wrappers

function db_q0($s,$a=0)
{
  GLOBAL $db;

  return $db->q0($s,$a);
}

function db_qok($s,$a=0)
{
  GLOBAL $db;

  $db->qok($s,$a);
}

function db_q1($s,$a=0)
{
  GLOBAL $db;

  return $db->q1($s,$a);
}

function db_q1all($s,$a=0)
{
  GLOBAL $db;

  return $db->q1all($s,$a);
}

function db_q1arr($s,$a=0)
{
  GLOBAL $db;

  return $db->q1arr($s,$a);
}

function db_qrow($s,$a=0)
{
  GLOBAL $db;

  return $db->qrow($s,$a);
}

function db_qall($s,$a=0)
{
  GLOBAL $db;

  return $db->qall($s,$a);
}

function db_qarr($s,$a=0)
{
  GLOBAL $db;

  return $db->qarr($s,$a);
}

function db_q2all($s,$a=0)
{
  GLOBAL $db;

  return $db->q2all($s,$a);
}

function db_q2arr($s,$a=0)
{
  GLOBAL $db;

  return $db->q2arr($s,$a);
}

function db_qxall($s,$a=0)
{
  GLOBAL $db;

  return $db->qxall($s,$a);
}

function db_qxarr($s,$a=0)
{
  GLOBAL $db;

  return $db->qxarr($s,$a);
}




function db_($s,$a=0)
{
  GLOBAL $db;

  return $db->_($s,$a);
}




function db_begin()
{
  GLOBAL $db;

  $db->begin();
}

function db_rollback()
{
  GLOBAL $db;

  $db->rollback();
}

function db_end()
{
  GLOBAL $db;

  $db->end();
}

function db_inc($t,$c)
{
  GLOBAL $db;

  return $db->inc($t, $c);
}

?>
