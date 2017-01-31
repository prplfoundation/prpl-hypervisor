# ====================================================================================
echo "    reb                         (Setup for an EJTAGBOOT indicated reset.)"
# ====================================================================================
proc reb {} {
    jtag_reset 0 1;irscan pic32mz.cpu 5;jtag_reset 0 0;irscan pic32mz.cpu 0x0c;
}

# ====================================================================================
echo "    rnb                         (Setup for a NORNALBOOT indicated reset.)"
# ====================================================================================
proc rnb {} {
    jtag_reset 0 1;	irscan pic32mz.cpu 5;jtag_reset 0 0;irscan pic32mz.cpu 0x0d;
}
