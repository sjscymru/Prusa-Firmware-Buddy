#include "req_parser.h"
#include "handler.h"
#include "server.h"

/*
 * The parsing automata. Generated by python scripts.
 */
#include <http_req_automaton.h>

using namespace automata;
using nhttp::parser::request::Names;
using std::string_view;

namespace {

const Automaton http_request(nhttp::parser::request::paths, nhttp::parser::request::transitions, nhttp::parser::request::states);

}

namespace nhttp::handler {

RequestParser::RequestParser(const Server &server)
    : Execution(&http_request)
    , server(&server) {}

ExecutionControl RequestParser::event(Event event) {
    switch (event.leaving_state) {
    case Names::MethodGet:
        method = Method::Get;
        return ExecutionControl::Continue;
    case Names::MethodHead:
        method = Method::Head;
        return ExecutionControl::Continue;
    case Names::MethodPost:
        method = Method::Post;
        return ExecutionControl::Continue;
    case Names::MethodDelete:
        method = Method::Delete;
        return ExecutionControl::Continue;
    case Names::MethodPut:
        method = Method::Put;
        return ExecutionControl::Continue;
    case Names::MethodUnknown:
        method = Method::Unknown;
        return ExecutionControl::Continue;
    }

    switch (event.entering_state) {
    case Names::Url:
        if (url_size < url.size()) {
            url[url_size++] = event.payload;
            return ExecutionControl::Continue;
        } else {
            error_code = Status::UriTooLong;
            return ExecutionControl::Continue;
        }
    case Names::XApiKey:
        // TODO
        return ExecutionControl::Continue;
    case Names::ContentLength:
        content_length = 10 * content_length + (event.payload - '0');
        return ExecutionControl::Continue;
    case Names::Body:
        done = true;
        return ExecutionControl::Stop;
    }

    return ExecutionControl::Continue;
}

Step RequestParser::step(string_view input, uint8_t *, size_t) {
    if (done) {
        return Step { 0, 0, Continue() };
    }

    const auto [result, consumed] = consume(input);

    if (result == ExecutionControl::NoTransition) {
        // Malformed request
        error_code = Status::BadRequest;
        done = true;
    }

    if (done) {
        /*
         * Note: we assume the array of selectors is terminated by one that
         * handles unhandled/unknown requests ‒ one that returns 404 or so.
         */
        for (const Selector *const *selector = server->selectors(); /* No condition on purpose*/; selector++) {
            if (auto state = (*selector)->accept(*this); state.has_value()) {
                return Step { consumed, 0, std::move(*state) };
            }
        }
    } else {
        return Step { consumed, 0, Continue() };
    }
}

}