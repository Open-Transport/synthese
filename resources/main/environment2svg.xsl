<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:svg="http://www.w3.org/2000/svg">
    
    <xsl:template match="/">
        <svg:svg xmlns="http://www.w3.org/2000/svg" version="1.0">
            
            <defs>
                <marker id="PStopMarker" viewBox="0 0 10 10" refX="5" refY="5" 
                    markerUnits="strokeWidth" 
                    markerWidth="3" 
                    markerHeight="3" orient="auto">
                    <svg:path d="M 0 0 L 10 0 L 10 10 L 0 10 z"/>
                </marker>
                <marker id="AddressMarker" viewBox="0 0 10 10" refX="5" refY="5" 
                    markerUnits="strokeWidth" 
                    markerWidth="3" 
                    markerHeight="3" orient="auto">
                    <svg:circle cx="5" cy="5" r="5"/>
                </marker>
                
            </defs>             
            
            <svg:g font-size="12" font-family="Verdana">
            <xsl:apply-templates/>
            </svg:g>
            
        </svg:svg>
    </xsl:template>
    
    
    <xsl:template match="environment">
        <xsl:apply-templates select="lines"/>
        <xsl:apply-templates select="roads"/>
        <xsl:apply-templates select="connectionPlaces"/>
    </xsl:template>
    
    
    <xsl:template match="lines">
        <xsl:apply-templates/>
    </xsl:template>
    
    
    <xsl:template match="roads">
        <xsl:apply-templates/>
    </xsl:template>
    
    
    <xsl:template match="connectionPlaces">
        <xsl:apply-templates/>
    </xsl:template>
    
    
    <xsl:template match="connectionPlace">
        
        <xsl:for-each select="transferDelay">
            <xsl:variable name="fromVertex" 
                select="ancestor::environment/physicalStops/*[@id=current()/@fromVertexId]"/>            
            <xsl:variable name="toVertex" 
                select="ancestor::environment/physicalStops/*[@id=current()/@toVertexId]"/>
            <svg:line 
                opacity="0.3"
                stroke-linecap="round"
                stroke="green" stroke-width="12"
                x1="{$fromVertex/@x}" y1="{$fromVertex/@y}"
                x2="{$toVertex/@x}" y2="{$toVertex/@y}"/>
            
        </xsl:for-each>
        
    </xsl:template>
    
    
    <xsl:template match="line">
            
        <!-- Draw and line to each physical stops -->
        <xsl:variable name="poly">
            <xsl:for-each select="ancestor::environment/lineStops/lineStop[@lineId=current()/@id]">
                <xsl:sort select="./@rankInPath" order="ascending" data-type="number"/>  
                <xsl:variable name="pstop" select="ancestor::environment/physicalStops/physicalStop[@id = current()/@physicalStopId]"/>
                <xsl:value-of select="$pstop/@x"/>,<xsl:value-of select="$pstop/@y"/><xsl:text> </xsl:text>
            </xsl:for-each>
        </xsl:variable>

        <xsl:if test="string-length ($poly)">
            <svg:polyline points="{$poly}" stroke="{@color}" style="fill:none;stroke-width:4"
                marker-start="url(#PStopMarker)" marker-mid="url(#PStopMarker)" marker-end="url(#PStopMarker)"/>
        
            <xsl:for-each select="ancestor::environment/lineStops/lineStop[@lineId=current()/@id]">
                <xsl:sort select="./@rankInPath" order="ascending" data-type="number"/>  
                
                <xsl:variable name="pstop" select="ancestor::environment/physicalStops/physicalStop[@id=current()/@physicalStopId]"/>
                
                <svg:text x="{$pstop/@x}" y="{$pstop/@y}" dx="5" dy="-5" font-size="75%" >
                    <xsl:value-of select="$pstop/@name"/> (<xsl:value-of select="@metricOffset"/>)
                </svg:text>
                
                <xsl:if test="$pstop/@placeId">
                    <svg:circle cx="{$pstop/@x}" cy="{$pstop/@y}" r="10" fill="green" opacity="0.3" stroke="{../@color}" stroke-width="1"/> 
                </xsl:if>
                    
                <!-- Draw the edge distance -->
                <xsl:if test="./@rankInPath > 0">
                    <xsl:variable name="previousls" select="../*[(@rankInPath = current()/@rankInPath - 1) and (current()/@lineId = @lineId)]"/>
                    <xsl:variable name="previousps" select="ancestor::environment/physicalStops/physicalStop[@id = $previousls/@physicalStopId]"/>
                    
                    <svg:circle cx="{($pstop/@x + $previousps/@x) div 2}" 
                                cy="{($pstop/@y + $previousps/@y) div 2}" 
                                r="8" fill="white" opacity="0.8" stroke="{../@color}" stroke-width="1"/> 
                    
                    <svg:text x="{($pstop/@x + $previousps/@x) div 2}" 
                              y="{($pstop/@y + $previousps/@y) div 2}" 
                              font-size="60%" text-anchor="middle" dominant-baseline="central">
                        <xsl:value-of select="@metricOffset - $previousls/@metricOffset"/>
                    </svg:text>
                    
                </xsl:if>
            </xsl:for-each>
            
        </xsl:if>
        
            
        
    </xsl:template>
    

    
 
    
    <xsl:template match="road">

        <!-- Draw and line to each physical stops -->
        <xsl:variable name="poly">
            <xsl:for-each select="ancestor::environment/addresses/address[@roadId = current()/@id]">
                <xsl:sort select="./@metricOffset" order="ascending" data-type="number"/>
                <xsl:value-of select="@x"/>,<xsl:value-of select="@y"/><xsl:text> </xsl:text>
            </xsl:for-each>
        </xsl:variable>
        
        <xsl:if test="string-length ($poly)">
            <svg:polyline points="{$poly}" stroke="grey" style="fill:none;stroke-width:4"
                marker-start="url(#PStopMarker)" marker-mid="url(#PStopMarker)" marker-end="url(#PStopMarker)"/>

            <xsl:variable name="addresses" select="ancestor::environment/addresses/address[@roadId = current()/@id]"/>
            <xsl:for-each select="$addresses/*">
                <xsl:sort select="./@metricOffset" order="ascending" data-type="number"/>
                
                <svg:text x="{@x}" y="{@y}" dx="5" dy="-5" font-size="75%" >
                    (<xsl:value-of select="@metricOffset"/>)
                </svg:text>
                
                <xsl:if test="@placeId">
                    <svg:circle cx="{@x}" cy="{@y}" r="10" fill="green" opacity="0.3" stroke="grey" stroke-width="1"/> 
                </xsl:if>
            
                <!-- Draw the edge distance -->
                <xsl:variable name="rank" select="position()"/>
                <xsl:if test="$rank > 1">
                    <xsl:variable name="previousad" select="$addresses/*[$rank - 1]"/>
                    
                    <svg:circle cx="{(@x + $previousad/@x) div 2}" 
                        cy="{(@y + $previousad/@y) div 2}" 
                        r="8" fill="white" opacity="0.8" stroke="black" stroke-width="1"/> 
                    
                    <svg:text x="{(@x + $previousad/@x) div 2}" 
                        y="{(@y + $previousad/@y) div 2}" 
                        font-size="60%" text-anchor="middle" dominant-baseline="central">
                        <xsl:value-of select="@metricOffset - $previousad/@metricOffset"/>
                    </svg:text>
                    
                </xsl:if>
                
            </xsl:for-each>
        </xsl:if>            
        
    </xsl:template>
    
    
    
    
    
    
</xsl:stylesheet>
