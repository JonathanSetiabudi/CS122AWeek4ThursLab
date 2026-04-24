module top (
    input logic clk,
    input wire sclk,
    input wire mosi,
    input wire cs_n,
    output reg [3:0] leds
);

    reg [7:0] shift_reg;
    reg [2:0] bit_count;

    initial begin
        shift_reg = 8'b00000000;
        bit_count = 3'b000;
        leds = 4'b0000;
    end

    // Sample MOSI on SCLK rising edge (SPI mode 0). Reset byte alignment when CS deasserts.
    always @(posedge sclk or posedge cs_n) begin
        if(cs_n == 1'b1) begin
            bit_count <= 3'b000;
        end else begin
            shift_reg <= {shift_reg[6:0], mosi};
            bit_count <= bit_count + 1;
            if (bit_count == 3'b111) begin
                // Master transmits nibble in data[7:4], so capture those 4 bits.
                leds <= shift_reg[6:3];
            end
        end
    end
endmodule