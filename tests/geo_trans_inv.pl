$er = 0;
open F, "geo_trans_inv 2>&1 |" or die;
while (<F>) {
  # print $_;
  if ($_ =~ /error has been detected/)
  {
    $er = 1;
    print "=============================================================\n";
    print $_, <F>;
  }
}
if ($er == 1) { exit(1); }
`geo_trans_inv`;
if ($?) { exit(1); }
