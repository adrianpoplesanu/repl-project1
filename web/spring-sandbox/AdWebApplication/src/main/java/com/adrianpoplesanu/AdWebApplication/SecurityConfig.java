package com.adrianpoplesanu.AdWebApplication;

import org.springframework.context.annotation.Configuration;
import org.springframework.security.config.annotation.web.builders.HttpSecurity;
import org.springframework.security.config.annotation.web.configuration.WebSecurityConfigurerAdapter;

import javax.servlet.http.HttpServletResponse;

@Configuration
public class SecurityConfig extends WebSecurityConfigurerAdapter {
    @Override
    protected void configure(HttpSecurity http) throws Exception {
        http.csrf().disable().authorizeRequests().anyRequest().permitAll();
        http.csrf().disable().logout(logout -> logout.permitAll()
            .logoutSuccessHandler(
                (request, response, authentication) -> {
                    response.setStatus(HttpServletResponse.SC_OK);
                }
            )
        );
    }
}
