`timescale 1ns/1ps
`include "src/top.sv"

module top_tb;

logic clk, btn_red, btn_blue, sw;
logic [6:0] seg;
logic dp;
logic led_red, led_blue;

top dut (.*);

always #20 clk = ~clk;

initial begin
    $dumpfile("build/ex7.vcd"); // intermediate file for waveform generation
    $dumpvars;                  // capture all signals under top_tb
    clk = 0; btn_red = 0; btn_blue = 0; sw = 0;
    
    #1000000 btn_red = 1;
    #50000000;
    btn_red = 0;
    #50000000;
    
    sw = 1;
    #1000000;
    
    btn_blue = 1;
    #50000000;
    btn_blue = 0;
    #50000000;
    
    $finish;
end

always @(posedge clk) begin
    if (dut.btn_red_out)
        $display("Time=%0t: Red duty_cycle=%d, sw=%b, display=%d, dp=%b", $time, dut.duty_cycle_red, sw, dut.display_value, dp);
    if (dut.btn_blue_out)
        $display("Time=%0t: Blue duty_cycle=%d, sw=%b, display=%d, dp=%b", $time, dut.duty_cycle_blue, sw, dut.display_value, dp);
end

endmodule