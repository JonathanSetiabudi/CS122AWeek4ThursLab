module top (
    input wire sclk,
    input wire mosi,
    input wire cs_n,
    output reg [3:0] leds
);

reg [7:0] shift_reg;
reg [2:0] bit_count;

initial begin
    shift_reg = 0;
    bit_count = 0;
    leds = 0;
end

// always @(posedge sclk) begin
//     if(cs_n == 0) begin
//         shift_reg <= {shift_reg[6:0], mosi};  // Shift in MOSI bit
//         bit_count <= bit_count + 1;
        
//     end else begin // Reset on CS high
//         shift_reg <= 0;
//         bit_count <= 0;
//     end

//     if (bit_count == 3'b111) begin
//         leds <= shift_reg[3:0];
//     end
// end

always @(posedge sclk) begin
    if (cs_n) begin
        shift_reg <= 0;
        bit_count <= 0;
    end else begin
        if (bit_count == 3'b111) begin
            // next_shift = {shift_reg[6:0], mosi}; low nibble is {shift_reg[2:0], mosi}
            leds <= {shift_reg[2:0], mosi};
            bit_count <= 0;
            shift_reg <= 0;
        end else begin
            shift_reg <= {shift_reg[6:0], mosi};
            bit_count <= bit_count + 1'b1;
        end
    end
end

endmodule