package com.adrianpoplesanu.AdWebApplication.Controller;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.ResponseBody;

@Controller
public class LoginController {
    @RequestMapping(value = "/authenticate", method = RequestMethod.POST)
    @ResponseBody
    String authenticate() {
        return "authentication response";
    }
}
